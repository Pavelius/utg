#include "code_command.h"
#include "code_rule.h"
#include "code_package.h"
#include "stringbuilder.h"

using namespace code;

namespace {
struct context {
	pckh		symbol, type, ast;
	unsigned	flags;
};
}

adat<pckh>	code::operations;
rulea		code::rules;
ruleopalla	code::binaryops;
ruleopa		code::unaryops;
ruleopa		code::postfixops;
static rule *unary_rule, *postfix_rule, *postfix_call_rule, *postfix_scope_rule, *postfix_initialize_rule;
fnerror		code::perror;
char		code::string_buffer[256 * 32];
context		context_data[32];
unsigned	context_current;

static adat<unsigned> locals;
static adat<symbol, 32> symbols;

static const char* file_source;
static const char* last_url;

const char*	code::p;
const char*	code::last_identifier;
const char*	code::last_position;
const char*	code::last_string;
pckh		code::last_ast;
static int	binary_level;
static symbol last_symbol_instance;
static symbol* last_symbol;

static context& getctx() {
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

void code::string() {
	if(*p == '\"') {
		stringbuilder sb(string_buffer);
		p = sb.psstr(p + 1, '\"');
		last_string = szdup(string_buffer);
		skipws();
	}
}

void code::number() {
	auto p1 = p;
	long value = 0;
	p = stringbuilder::read(p, value);
	if(p1 != p) {
		skipws();
		operations.add(last_package->add(operation::Number, value));
	}
}

void code::identifier() {
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

static void parse_token(const token& e);

static void parse_rule(const rule& v) {
	auto p0 = p;
	auto need_stop = false;
	auto push_context = context_current;
	for(auto& e : v.tokens) {
		if(!e)
			break;
		if(e.is(flag::Stop))
			need_stop = true;
		auto p1 = p;
		parse_token(e);
		if(e.is(flag::Execute))
			continue;
		if(p1 == p) {
			// Token did not work
			if(need_stop || e.is(flag::Condition)) // If tokens is optional continue parse next token
				continue;
			// TODO: single_statement fix. And how debug this?
			p = p0; // This rule is invalid, rollback all and exit
			return;
		}
		if(need_stop)
			break;
	}
	context_current = push_context;
	if(need_stop && p0 == p)
		return; // We need 'one of' tokens and not gain valid token at exit
	if(v.apply) {
		if((p0 != p) || !v.tokens[0]) {
			last_position = p0;
			v.apply(); // Only valid token execute proc
		}
	}
}

static void parse_token(const token& e) {
	if(e.command)
		e.command->proc();
	else if(e.is(flag::Execute)) {
		if(e.rule && e.rule->apply)
			e.rule->apply();
	} else if(e.rule) {
		auto p0 = p;
		parse_rule(*e.rule);
		if(e.is(flag::Repeat)) {
			auto p2 = p0;
			while(p2 != p) {
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
				if(p3 == p) {
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
		auto n = zlen(e.id);
		if(memcmp(p, e.id, n) != 0)
			return;
		last_token = &e;
		p += n;
		skipws();
	}
}

void code::binary_operation(operation op) {
	if(operations.getcount() < 2)
		error("Operations stack corrupt when parse binare operation in `%1`", example(p));
	auto n = operations.count;
	operations.data[n - 2] = last_package->add(op, operations.data[n - 2], operations.data[n - 1]);
	operations.count--;
}

void code::unary_operation(operation op) {
	if(operations.getcount() < 1)
		error("Operations stack corrupt when parse unary operation in `%1`", example(p));
	auto n = operations.count;
	operations.data[n - 2] = last_package->add(op, operations.data[n - 2], operations.data[n - 1]);
	operations.count--;
}

static bool match(const char* symbol) {
	auto i = 0;
	while(symbol[i]) {
		if(p[i] != symbol[i])
			return false;
		i++;
	}
	p += i;
	skipws();
	return true;
}

void code::skip(const char* symbol) {
	if(!match(symbol))
		error("Expected token `%1` in `%2`", symbol, example(p));
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

static operation match_operation(const ruleopa& source) {
	for(auto& e : source) {
		if(match(e.id))
			return e.value;
	}
	return operation::None;
}

static void parse_postfix() {
	while(true) {
		switch(*p) {
		case '(': // Calling
			skip(")");
			return; // Must return
		case '[': // Scoping
			skipws(1);
			parse_expression();
			skip("]");
			break;
		case '{': // Initializing
			skipws(1);
			parse_expression();
			skip("}");
			return; // Must return
		default:
			auto op = match_operation(postfixops);
			if(op == operation::None)
				return;
			unary_operation(op);
			break;
		}
	}
}

static void parse_binary();

static void parse_unary() {
	if(*p == '(') {
		skipws(1);
		parse_binary();
		skip(")");
	} else {
		while(true) {
			auto op = match_operation(unaryops);
			if(op == operation::None)
				break;
			parse_unary();
			unary_operation(op);
		}
	}
}

static void parse_binary(int level) {
	if(level < 0)
		parse_unary();
	else {
		parse_binary(level - 1);
		while(true) {
			auto op = match_operation(binaryops.begin()[level]);
			if(op == operation::None)
				break;
			parse_binary(level - 1);
			binary_operation(op);
		}
	}
}

static void parse_binary() {
	parse_binary(binary_level);
}

static rule* find_rule(const char* id, bool need_error = false) {
	for(auto& e : rules) {
		if(strcmp(e.id, id) == 0)
			return &e;
	}
	if(need_error)
		error("Not found rule `%1`", id);
	return 0;
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
	context_current++;
}

static void add_type() {
	auto id_result = last_package->add(last_url);
	auto id = last_package->add(last_identifier);
	getctx().type = last_package->add(id, Modules, id_result, code::last_position - file_source, 0, 0);
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
	auto& e = getctx();
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

static void set_static() {
	last_symbol->flags |= FG(Static);
}

static void set_public() {
	last_symbol->flags |= FG(Public);
}

static void set_symbol_ast() {
	auto& e = getctx();
	auto ps = last_package->getsym(e.symbol);
	if(ps)
		ps->ast = e.ast;
}

static void type_reference() {
	getctx().type = last_package->reference(getctx().type);
}

static void push_locals() {
	locals.add(code::p - file_source);
}

static void pop_locals() {
	if(!locals.count)
		error("Closing scope bracing without opening one");
	else
		locals.count--;
}

static void expression() {
	parse_expression();
	if(!operations)
		error("Expected operation when parse `%1`", example(p));
	else
		getctx().ast = operations.data[operations.count - 1];
}

static command default_commands[] = {
	{"add_type", add_type},
	{"add_member", add_member},
	{"identifier", identifier},
	{"expression", expression},
	{"number", number},
	{"pop_locals", pop_locals},
	{"push_context", push_context},
	{"push_locals", push_locals},
	{"string", string},
	{"set_public", set_public},
	{"set_static", set_static},
	{"set_symbol_ast", set_symbol_ast},
};

static const command* find_command(const char* id) {
	for(auto& e : default_commands) {
		if(equal(e.id, id))
			return &e;
	}
	return 0;
}

static bool lazy_initialize() {
	for(auto& r : rules) {
		for(auto& e : r.tokens) {
			if(!e)
				break;
			if(e.rule)
				return false; // All rules initialized
			if(e.is(flag::Variable) || e.is(flag::Execute)) {
				e.rule = find_rule(e.id);
				if(!e.rule)
					error("In rule `%1` not found token `%2`", r.id, e.id);
			} else if(e.is(flag::Execute)) {
				e.command = find_command(e.id);
				if(!e.command)
					error("In rule `%1` not found command `%2`", r.id, e.id);
			}
		}
	}
	return true;
}

void code::setrules(rulea source) {
	rules = source;
	auto first_time = lazy_initialize();
	// Initialize common rules
	postfix_rule = find_rule("postfix", first_time);
	unary_rule = find_rule("unary", first_time);
	//postfix_scope_rule = find_rule("postfix_scope", first_time);
	//postfix_call_rule = find_rule("postfix_call", first_time);
	//postfix_initialize_rule = find_rule("postfix_initialize", first_time);
}