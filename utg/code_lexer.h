#include "code_parser.h"

#pragma once

namespace code {
struct lexer {
	const char*	id;
	const char*	files;
	rulea		rules;
	static void read(const char* url);
	void		activate();
};
extern lexer* last_lexer;
}
