#include "code_rule.h"

#pragma once

namespace code {
struct lexer {
	const char*	id;
	const char*	files;
	rulea		rules;
	ruleopa		binary, unary, postfix;
	ruleopa		binary_priority[10];
	static void read(const char* url);
};
extern lexer* last_lexer;
}
