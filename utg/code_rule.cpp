#include "code_rule.h"
#include "code_package.h"
#include "stringbuilder.h"

using namespace code;

adat<pckh>	code::operations;
rulea		code::rules;
static rule *unary_rule, *postfix_rule;
fnerror		code::perror;
char		code::string_buffer[256 * 32];

const char*	code::p;
const char*	code::last_identifier;
const char*	code::last_position;
const char*	code::last_string;
pckh		code::last_ast;

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
	long value  = 0;
	p = stringbuilder::read(p, value);
	if(p1 != p) {
		skipws();
		last_ast = last_package->add(operation::Number, value);
		operations.add(last_ast);
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

operation code::parse_operation(const char* p) {
	if(p[0] == '<') {
		if(p[1] == '=')
			return operation::LessEqual;
		else if(p[1] == '<')
			return operation::ShiftLeft;
		else
			return operation::Less;
	} else if(p[0] == '>') {
		if(p[1] == '=')
			return operation::GreaterEqual;
		else if(p[1] == '>')
			return operation::ShiftRight;
		else
			return operation::Greater;
	} else if(p[0] == '|') {
		if(p[1] == '|')
			return operation::Or;
		else
			return operation::BinaryOr;
	} else if(p[0] == '&') {
		if(p[1] == '&')
			return operation::And;
		else
			return operation::BinaryAnd;
	} else if(p[0] == '^')
		return operation::BinaryÕîr;
	else if(p[0] == '!') {
		if(p[1] == '=')
			return operation::NotEqual;
		else
			return operation::Not;
	} else if(p[0] == '=') {
		if(p[1] == '=')
			return operation::Equal;
		else
			return operation::Assign;
	} else if(p[0] == '/')
		return operation::Div;
	else if(p[0] == '*')
		return operation::Mul;
	else if(p[0] == '+')
		return operation::Plus;
	else if(p[0] == '-')
		return operation::Minus;
	else
		return operation::None;
}

static void parse_token(const token& e);

static void parse_rule(const rule& v) {
	auto need_stop = false;
	auto p0 = p;
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
	if(e.is(flag::Execute)) {
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
		p += 1;
		skipws();
		parse_expression();
		skip(":");
		parse_expression();
	}
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
			}
		}
	}
	return true;
}

void code::parse(const char* source_code, const char* rule_id) {
	if(!rule_id)
		rule_id = "global";
	auto pr = find_rule(rule_id);
	if(!pr) {
		error("Not found rule `%1`", rule_id);
		return;
	}
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

void code::setrules(rulea source) {
	rules = source;
	auto first_time = lazy_initialize();
	// Initialize common rules
	postfix_rule = find_rule("postfix", first_time);
	unary_rule = find_rule("unary", first_time);
}

void code::parse(const char* source_code, const char* rule_id, rulea source) {
	setrules(source);
	parse(source_code, rule_id);
}
