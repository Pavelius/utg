#include "bsreq.h"
#include "code_lexer.h"
#include "log.h"

using namespace code;

lexer* code::last_lexer;

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
		error("Define only one lexer in one file");
		return;
	}
	auto push_lexer = last_lexer;
	last_lexer = (lexer*)bsdata<lexer>::source.ptr(start_lexer);
	last_lexer->rules = sliceu<rule>(start_rules, end_rules - start_rules);
	initialize_all();
	last_lexer = push_lexer;
}

void lexer::activate() {
	last_lexer = this;
	setrules(last_lexer->rules);
}
