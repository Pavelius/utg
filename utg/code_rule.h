#include "crt.h"
#include "code_token.h"

#pragma once

namespace code {
struct rule {
	const char*			id;
	tokena				tokens;
	fnevent				special;
};
typedef slice<rule>		rulea;
extern rulea			this_rules;
extern const char*		p;
void					errorv(const char* format, const char* format_param);
void					error(const char* format, ...);
void					parse(const char* source_code, rulea source, const char* rule);
void					skipws();
}