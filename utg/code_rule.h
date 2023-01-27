#include "crt.h"
#include "code_token.h"

#pragma once

namespace code {

struct rule {
	const char* id;
	tokena		tokens;
	fnevent		determinal;
};

typedef slice<rule> rulea;
typedef void(*fnerror)(const char* position, const char* format, const char* format_param);

extern char	string_buffer[256 * 32];
extern rulea rules;
extern fnerror perror;
extern const char* p;
extern const char* last_identifier;
extern const char* last_string;
extern long last_value;

void identifier();
void number();
void string();

void errorv(const char* position, const char* format, const char* format_param);
void error(const char* format, ...);
void parse(const char* source_code, const char* rule_id, rulea source);
void parse(const char* source_code, const char* rule_id = 0);
void setrules(rulea source);
void skipws();

}