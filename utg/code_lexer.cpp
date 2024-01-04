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
	last_lexer->activate();
	initialize_rules();
	last_lexer = push_lexer;
}

void lexer::activate() {
	last_lexer = this;
	setrules(last_lexer->rules);
}
