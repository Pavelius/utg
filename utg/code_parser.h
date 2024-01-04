#include "code_package.h"
#include "variant.h"

#pragma once

namespace code {
enum class flag : unsigned char {
	Variable, Condition, Repeat, ComaSeparated, PointSeparated, Stop, Execute
};
struct token {
	const char*	id;
	unsigned	flags;
	const struct rule* rule;
	const struct command* command;
	unsigned	param;
	constexpr explicit operator bool() const { return id != 0; }
	bool		is(flag v) const { return (flags & (1 << (int)v)) != 0; }
	void		set(flag v) { flags |= 1 << (int)v; }
};
extern const token*	last_token;
typedef token tokena[16];
struct rule {
	const char* id;
	tokena		tokens;
};
struct evalue {
	operation	type;
	long		value;
	array*		section;
};
typedef adat<pckh> operationa;
typedef sliceu<rule> rulea;
typedef void(*fnerror)(const char* position, const char* format, const char* format_param);

extern char	string_buffer[256 * 32];
extern rulea rules;
extern fnerror perror;

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

void initialize_rules();
void errorv(const char* position, const char* format, const char* format_param);
void error(const char* format, ...);
void parse(const char* source_code, const char* rule_id = 0);
void parse_expression();
void setrules(rulea source);
void skip(const char* symbols);
void skipws();
void skipws(int n);

operation parse_operation(const char* p);

}