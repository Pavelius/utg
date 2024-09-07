#include "bsdata.h"
#include "code_command.h"
#include "code_parser.h"
#include "flagable.h"
#include "stringbuilder.h"

using namespace code;

BSDATAC(rule, 1024)

namespace {
struct context {
	pckh		symbol, type;
	const char*	start;
	unsigned	flags;
	void clear() {
		symbol = type = None;
		flags = 0;
		start = 0;
	}
};
static context	context_data[32];
static unsigned	context_current;
struct rulectx;
static rulectx* last;
struct rulectx {
	const struct token* token;
	const struct rule& rule;
	rulectx*	previous;
	const char*	position;
	unsigned	symbol;
	rulectx(const struct rule& rule) : rule(rule), previous(last), position(last_position), symbol(context_current), token(0) { last_position = p; last = this; }
	~rulectx() { last_position = position; context_current = symbol; last = previous; }
};
}

adat<pckh>	code::operations;
rulea		code::rules;
fnerror		code::perror;
char		code::string_buffer[256 * 32];

static rule	*unary_rule, *postfix_rule;
static adat<unsigned> locals;
static adat<unsigned> locals_ops;
static pckh	last_url;

static const char* file_source;

const char* code::p;
const char* code::last_identifier;
const char* code::last_position;
const char*	code::last_string;

static context& geti() {
	return context_data[context_current];
}

const char* code::example(const char* p) {
	static char temp[40]; stringbuilder sb(temp);
	sb.psstrlf(p);
	return temp;
}

void code::errorv(const char* p, const char* format, const char* format_param) {
	if(perror)
		perror(p, format, format_param);
}

void code::error(const char* format, ...) {
	errorv(p, format, xva_start(format));
}

static void comments() {
	if(p[0] == '/' && p[1] == '/') {
		p += 2;
		while(*p && *p != 10 && *p != 13)
			p++;
		p = skipspcr(p);
	} else if(p[0] == '/' && p[1] == '*') {
		p += 2;
		while(*p) {
			if(p[0] == '*' && p[1] == '/') {
				p = skipspcr(p + 2);
				break;
			} else
				p++;
		}
	}
}

static void string() {
	if(*p == '\"') {
		stringbuilder sb(string_buffer);
		p = sb.psstr(p + 1, '\"');
		last_string = szdup(string_buffer);
		skipws();
	}
}

static void number() {
	auto p1 = p;
	long value = 0;
	p = psnum(p, value);
	if(p1 != p) {
		skipws();
		operations.add(last_package->add(operation::Number, value));
	}
}

static void identifier() {
	auto p1 = p;
	stringbuilder sb(string_buffer);
	p = sb.psidf(p);
	if(p1 != p) {
		last_identifier = szdup(string_buffer);
		skipws();
	}
}

void code::skipws() {
	while(*p) {
		p = skipspcr(p);
		auto p1 = p;
		comments();
		if(p1 != p)
			continue;
		break;
	}
}

void code::skipws(int n) {
	p += n;
	skipws();
}

static void parse_rule(const rule& v) {
	rulectx push(v);
	auto need_stop = false;
	for(auto& e : v.tokens) {
		if(!e)
			break;
		last->token = &e;
		if(e.is(flag::Stop))
			need_stop = true;
		auto p1 = p;
		if(e.command) {
			// Command execute and not checked later
			e.command->proc();
			continue;
		} else if(e.rule) {
			parse_rule(*e.rule);
			if(e.is(flag::Repeat)) {
				auto p2 = p1;
				while(p > p2) {
					p2 = p;
					auto required = false;
					if(e.is(flag::ComaSeparated)) {
						if(p[0] == ',') {
							p++; skipws();
							required = true;
						}
					} else if(e.is(flag::PointSeparated)) {
						if(p[0] == '.') {
							p++; skipws();
							required = true;
						}
					}
					auto p3 = p;
					parse_rule(*e.rule);
					if(p <= p3) {
						if(required)
							error("Expected %1", e.id);
					} else {
						if(!required) {
							if(e.is(flag::ComaSeparated))
								error("Expected symbol `,` when parse `%1`", example(p2));
							else if(e.is(flag::PointSeparated))
								error("Expected symbol `.` when parse `%1`", example(p2));
						}
					}
				}
			}
		} else {
			// Speed up by cashing string len in `param`
			if(memcmp(p, e.id, e.param) == 0) {
				p += e.param;
				skipws();
			}
		}
		if(p1 >= p) {
			// Token did not work
			if(need_stop || e.is(flag::Condition)) // If tokens is optional continue parse next token
				continue;
			// TODO: single_statement fix. And how debug this?
			p = last_position; // This rule is invalid, rollback all and exit
			break;
		}
		if(need_stop)
			break;
	}
}

static rule* find_rule(const char* id) {
	for(auto& e : rules) {
		if(equal(e.id, id))
			return &e;
	}
	error("Not found rule `%1`", id);
	return 0;
}

static const char* get_param(const char* p) {
	static char temp[260];
	temp[0] = 0; stringbuilder sb(temp);
	sb.psidf(p);
	return temp;
}

static void parse_token_name(token& e, const char* p) {
	while(*p) {
		if(*p == '\\') {
			p++;
			break;
		} else if(p[0] == ',' && p[1] == ' ') {
			e.set(flag::ComaSeparated);
			e.set(flag::Repeat);
			p++;
		} else if(p[0] == '.' && p[1] == ' ') {
			e.set(flag::PointSeparated);
			e.set(flag::Repeat);
			p++;
		} else if(*p == '^')
			e.set(flag::Stop);
		else if(*p == '.')
			e.set(flag::Repeat);
		else if(*p == '%')
			e.set(flag::Variable);
		else if(*p == '?')
			e.set(flag::Condition);
		else if(*p == '@')
			e.set(flag::Execute);
		else
			break;
		p++;
	}
	e.id = p;
	// Parse parameters
	while(true) {
		auto p_next = zchr(p, '.');
		if(!p_next)
			break;
		auto pn = get_param(p);
		e.param = getbsi(bsdata<flagi>::find(pn));
		if(e.param == 0xFFFF)
			e.param = getbsi(bsdata<operationi>::find(pn));
		if(e.param == 0xFFFF) {
			e.param = 0;
			error("In token `%1` not found parameter `%2`", e.id, pn);
		}
		p = p_next + 1;
	}
	e.id = p;
}

static void initialize_tokens() {
	for(auto& r : rules) {
		for(auto& e : r.tokens) {
			if(!e)
				break;
			parse_token_name(e, e.id);
		}
	}
}

static void initialize_tokens_depency() {
	for(auto& r : rules) {
		for(auto& e : r.tokens) {
			if(!e)
				break;
			if(e.is(flag::Variable)) {
				e.rule = find_rule(e.id);
				if(!e.rule)
					error("In rule `%1` not found token `%2`", r.id, e.id);
			} else if(e.is(flag::Execute)) {
				e.command = bsdata<command>::find(e.id);
				if(!e.command)
					error("In rule `%1` not found command `%2`", r.id, e.id);
			} else
				e.param = zlen(e.id);
		}
	}
}

void code::initialize_rules() {
	initialize_tokens();
	initialize_tokens_depency();
}

void code::binary_operation(operation op) {
	if(operations.getcount() < 2)
		error("Binary operations stack corrupt in `%1`", example(p));
	auto n = operations.count;
	operations.data[n - 2] = last_package->add(op, operations.data[n - 2], operations.data[n - 1]);
	operations.count--;
}

void code::unary_operation(operation op) {
	if(operations.getcount() < 1)
		error("Unary operations stack corrupt in `%1`", example(p));
	auto n = operations.count;
	operations.data[n - 1] = last_package->add(op, operations.data[n - 1]);
}

static void unary() {
	switch(p[0]) {
	case '-':
		if(p[1] == '-') {
			skipws(2);
			unary();
		} else {
			skipws(1);
			unary();
			unary_operation(operation::Neg);
		}
		break;
	case '+':
		if(p[1] == '+') {
			skipws(2);
			unary();
		} else {
			skipws(1);
			unary();
		}
		break;
	case '!':
		skipws(1);
		unary();
		unary_operation(operation::Not);
		break;
	case '*':
		skipws(1);
		unary();
		unary_operation(operation::Dereference);
		break;
	case '&':
		skipws(1);
		unary();
		unary_operation(operation::AdressOf);
		break;
	case '(':
		skipws(1);
		parse_expression();
		skip(")");
		break;
	default:
		parse_rule(*unary_rule);
		break;
	}
	parse_rule(*postfix_rule);
}

static void multiplication() {
	unary();
	while((p[0] == '*' || p[0] == '/' || p[0] == '%') && p[1] != '=') {
		char s = p[0]; skipws(1);
		operation op;
		switch(s) {
		case '/': op = operation::Div; break;
		case '%': op = operation::DivRest; break;
		default: op = operation::Mul; break;
		}
		unary();
		binary_operation(op);
	}
}

static void addiction() {
	multiplication();
	while((p[0] == '+' || p[0] == '-') && p[1] != '=') {
		char s = p[0]; skipws(1);
		operation op;
		switch(s) {
		case '+': op = operation::Plus; break;
		case '-': op = operation::Minus; break;
		default: op = operation::Mul; break;
		}
		multiplication();
		binary_operation(op);
	}
}

static void binary_cond() {
	addiction();
	while((p[0] == '>' && p[1] != '>')
		|| (p[0] == '<' && p[1] != '<')
		|| (p[0] == '=' && p[1] == '=')
		|| (p[0] == '!' && p[1] == '=')) {
		char t1 = *p++;
		char t2 = 0;
		if(p[0] == '=')
			t2 = *p++;
		skipws();
		operation op;
		switch(t1) {
		case '>':
			op = operation::Greater;
			if(t2 == '=')
				op = operation::GreaterEqual;
			break;
		case '<':
			op = operation::Less;
			if(t2 == '=')
				op = operation::LessEqual;
			break;
		case '!': op = operation::NotEqual; break;
		default: op = operation::Equal; break;
		}
		addiction();
		binary_operation(op);
	}
}

static void binary_and() {
	binary_cond();
	while(p[0] == '&' && p[1] != '&') {
		skipws(1);
		binary_cond();
		binary_operation(operation::BinaryAnd);
	}
}

static void binary_xor() {
	binary_and();
	while(p[0] == '^') {
		skipws(1);
		binary_and();
		binary_operation(operation::BinaryÕîr);
	}
}

static void binary_or() {
	binary_xor();
	while(p[0] == '|' && p[1] != '|') {
		skipws(1);
		binary_xor();
		binary_operation(operation::BinaryOr);
	}
}

static void binary_shift() {
	binary_or();
	while((p[0] == '>' && p[1] == '>') || (p[0] == '<' && p[1] == '<')) {
		operation op;
		switch(p[0]) {
		case '>': op = operation::ShiftRight; break;
		default: op = operation::ShiftLeft; break;
		}
		skipws(2);
		binary_or();
		binary_operation(op);
	}
}

static void logical_and() {
	binary_shift();
	while(p[0] == '&' && p[1] == '&') {
		skipws(2);
		binary_shift();
		binary_operation(operation::And);
	}
}

static void logical_or() {
	logical_and();
	while(p[0] == '|' && p[1] == '|') {
		skipws(2);
		logical_and();
		binary_operation(operation::Or);
	}
}

void code::parse_expression() {
	logical_or();
	while(*p == '?') {
		skipws(1);
		parse_expression();
		skip(":");
		parse_expression();
		binary_operation(operation::LeftIfTrue);
		binary_operation(operation::BooleanChoose);
	}
}

void code::parse(const char* source_code, const char* rule_id) {
	if(!rule_id)
		rule_id = "global";
	auto pr = find_rule(rule_id);
	if(!pr) {
		error("Not found rule `%1`", rule_id);
		return;
	}
	file_source = source_code;
	p = source_code;
	skipws();
	while(*p) {
		auto pb = p;
		parse_rule(*pr);
		if(pb == p) {
			error("Can't parse `%1`", example(pb));
			return;
		}
	}
}

static void push_context() {
	if(context_current >= sizeof(context_data) / sizeof(context_data[0]))
		error("Context stack corrupt");
	if(context_current < sizeof(context_data) / sizeof(context_data[0]) - 1)
		context_data[context_current + 1] = context_data[context_current];
	context_current++;
}

static void add_type() {
	auto id = last_package->add(last_identifier);
	geti().type = last_package->add(id, Modules, last_url, code::last_position - file_source, 0, 0);
}

static void add_member() {
	auto scope = 0;
	if(locals)
		scope = locals.data[locals.count - 1];
	auto id = last_package->add(last_identifier);
	auto symbol = last_package->findsymscope(id, This, scope);
	if(symbol != None) {
		error("Symbol `%1` already defined", last_identifier);
		return;
	}
	auto& e = geti();
	e.symbol = last_package->add(id, This, e.type, code::last_position - file_source, e.flags, scope);
}

static void add_variable() {
	auto id = last_package->add(last_identifier);
	pckh result = None;
	for(int i = locals.count - 1; (result == None) && i >= 0; i--)
		result = last_package->findsymscope(id, This, locals.data[i]);
	if(result == None)
		result = last_package->findsymscope(id, This, 0);
	if(result == None)
		result = last_package->findsym(id, Modules);
	if(result == None)
		error("Undefined identifier `%1`", last_identifier);
	operations.add(last_package->add(operation::Identifier, result));
}

static void set_flag() {
	geti().flags |= FG(last->token->param);
}

static void begin_string() {
	geti().start = code::p;
}

static pckh get_string(const char* id) {
	char temp[512];
	if(!geti().start) {
		error("Missing `@begin_string` token in %1 naming.", id);
		return None;
	}
	size_t len = code::p - geti().start, max = sizeof(temp) / sizeof(temp[0]) - 1;
	if(len > max) {
		error("%3 is too long. You have %1i bytes and must be %2i bytes.", len, max, id);
		return None;
	}
	memcpy(temp, geti().start, len);
	temp[len] = 0;
	return last_package->add(temp);
}

static void set_url() {
	last_url = get_string("url");
}

static void set_type() {
	auto& e = geti();
	if(equal(last_identifier, "int") || equal(last_identifier, "bool"))
		e.type = i32;
	else if(equal(last_identifier, "char"))
		e.type = i8;
	else if(equal(last_identifier, "short"))
		e.type = i16;
	else if(equal(last_identifier, "unsigned"))
		e.type = u32;
	else if(equal(last_identifier, "void"))
		e.type = Void;
	else {
		e.type = last_package->findsym(last_identifier, Modules);
		if(geti().type == None) {
			e.type = i32;
			p = last_position;
		}
	}
}

static void set_symbol_ast() {
	auto& e = geti();
	auto ps = last_package->getsym(e.symbol);
	if(ps) {
		if(operations.getcount() == 1) {
			ps->ast = operations.data[0];
			operations.count--;
		} else if(operations.getcount() > 1)
			error("Operations stack corrupted when add symbol result");
	}
}

static void type_reference() {
	geti().type = last_package->reference(geti().type);
}

static void push_locals() {
	locals.add(code::p - file_source);
	locals_ops.add(operations.getcount());
}

static void pop_locals() {
	if(!locals.count)
		error("Closing scope bracing without opening one");
	else {
		locals.count--;
		locals_ops.count--;
	}
}

static void expression() {
	parse_expression();
	if(!operations)
		error("Expected operation when parse `%1`", example(p));
}

static void declaration() {
	push_context();
	geti().clear();
}

void code::setrules(rulea source) {
	rules = source;
	postfix_rule = find_rule("postfix");
	unary_rule = find_rule("unary");
}

static void add_binary() {
	binary_operation((operation)last->token->param);
}

static void add_unary() {
	unary_operation((operation)last->token->param);
}

static void add_statement() {
	auto count = operations.getcount();
	if(locals_ops.getcount() > 0)
		count -= locals_ops.data[locals_ops.getcount() - 1];
	switch(count) {
	case 0: return; // Empthy statement
	case 1: return; // One operation, statement not need
	case 2: binary_operation(operation::Statement); break;
	default: error("Can't add emphty statement"); break;
	}
}

static void add_operator() {
	operations.add(last_package->add((operation)last->token->param, 0));
}

static void debug() {
}

BSDATA(command) = {
	{"add_binary", add_binary},
	{"add_member", add_member},
	{"add_operator", add_operator},
	{"add_statement", add_statement},
	{"add_type", add_type},
	{"add_unary", add_unary},
	{"add_variable", add_variable},
	{"begin_string", begin_string},
	{"debug", debug},
	{"identifier", identifier},
	{"expression", expression},
	{"declaration", declaration},
	{"number", number},
	{"pop_locals", pop_locals},
	{"push_context", push_context},
	{"push_locals", push_locals},
	{"string", string},
	{"set_flag", set_flag},
	{"set_symbol_ast", set_symbol_ast},
	{"set_url", set_url},
	{"set_type", set_type},
	{"type_reference", type_reference},
};
BSDATAF(command)