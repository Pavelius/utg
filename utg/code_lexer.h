#include "code_rule.h"

#pragma once

namespace code {
struct lexer {
	const char*		id;
	const char*		files;
	rulea			rules;
	ruleopa			operations;
	ruleopa			operations_priority[10];
	void			initialize();
};
extern lexer* last_lexer;
}
