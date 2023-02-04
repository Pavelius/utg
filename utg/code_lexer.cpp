#include "bsreq.h"
#include "code_lexer.h"
#include "log.h"

using namespace code;

lexer* code::last_lexer;

static int getpriority(operation t) {
	switch(t) {
	case operation::Mul:
	case operation::Div: case operation::DivRest:
		return 0;
	case operation::Plus: case operation::Minus:
		return 1;
	case operation::Greater: case operation::Less:
	case operation::Equal: case operation::NotEqual:
		return 2;
	case operation::BinaryAnd: case operation::BinaryOr: case operation::BinaryÕîr:
		return 3;
	case operation::ShiftLeft: case operation::ShiftRight:
		return 4;
	case operation::And:
		return 5;
	case operation::Or:
		return 6;
	default:
		return 7;
	}
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
			auto op = match_operation(last_lexer->postfix);
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
			auto op = match_operation(last_lexer->unary);
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
			auto op = match_operation(last_lexer->binary_priority[level]);
			if(op == operation::None)
				break;
			parse_binary(level - 1);
			binary_operation(op);
		}
	}
}

static void parse_binary() {
	parse_binary(7);
}

static int compare_rule(const void* pv1, const void* pv2) {
	auto p1 = (rule*)pv1;
	auto p2 = (rule*)pv2;
	return strcmp(p1->id, p2->id);
}

static rule* find_rule(const char* id, bool need_error = false) {
	for(auto& e : last_lexer->rules) {
		if(strcmp(e.id, id) == 0)
			return &e;
	}
	if(need_error)
		error("Not found rule `%1`", id);
	return 0;
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
		else {
			break;
		}
		p++;
	}
	e.id = p;
}

static void initialize_tokens() {
	for(auto& r : last_lexer->rules) {
		for(auto& e : r.tokens) {
			if(!e)
				break;
			parse_token_name(e, e.id);
		}
	}
}

static void initialize_rules() {
	for(auto& r : last_lexer->rules) {
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
			}
		}
	}
}

static void initialize_all() {
	initialize_tokens();
	initialize_rules();
}

void lexer::read(const char* url) {
	auto start_rules = bsdata<rule>::source.getcount();
	auto start_lexer = bsdata<lexer>::source.getcount();
	bsreq::read(url);
	if(log::geterrors())
		return;
	auto end_lexer = bsdata<lexer>::source.getcount();
	auto end_rules = bsdata<rule>::source.getcount();
	if(start_lexer == end_lexer) {
		error("Define one lexer");
		return;
	} else if((end_lexer - start_lexer) > 1) {
		error("Define only one lexer in ont file");
		return;
	}
	auto push_lexer = last_lexer;
	last_lexer = (lexer*)bsdata<lexer>::source.ptr(start_lexer);
	last_lexer->rules = sliceu<rule>(start_rules, end_rules - start_rules);
	initialize_all();
	last_lexer = push_lexer;
}
