#include "code_package.h"
#include "code_token.h"

#pragma once

namespace code {

struct rule {
	const char* id;
	tokena		tokens;
	fnevent		apply;
};
struct evalue {
	operation	type;
	long		value;
	array*		section;
};

typedef adat<pckh> operationa;
typedef slice<rule> rulea;
typedef void(*fnerror)(const char* position, const char* format, const char* format_param);

extern char	string_buffer[256 * 32];
extern rulea rules;
extern fnerror perror;

extern pckh	last_ast;

extern operationa operations;

extern const char* p;
extern const char* last_identifier;
extern const char* last_position;
extern const char* last_string;

const char* example(const char* p);

void identifier();
void number();
void string();

void binary_operation(operation op);
void unary_operation(operation op);

void errorv(const char* position, const char* format, const char* format_param);
void error(const char* format, ...);
void parse(const char* source_code, const char* rule_id, rulea source);
void parse(const char* source_code, const char* rule_id = 0);
void parse_expression();
void setrules(rulea source);
void skip(const char* symbols);
void skipws();
void skipws(int n);

operation parse_operation(const char* p);

}