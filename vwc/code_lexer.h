#include "code_rule.h"

#pragma once

namespace code {
struct lexer {
	const char*		id;
	const char*		files;
	rulea			rules;
	ruleopa			unaryops;
	ruleopalla		binaryops;
};
extern lexer* last_lexer;
}
