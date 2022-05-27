#include "crt.h"
#include "code_token.h"

#pragma once

namespace code {
struct parser {
	const char*			p;
	int					param[3];
	constexpr parser(const char* p) : p(p), param() {}
};
struct rule {
	enum {
		None,
		SetValue, SetValueRange,
		Determinal, UnaryOperation, BinaryOperation,
	};
	const char*			id;
	tokena				tokens;
	int					type, param;
	void				apply(parser& v) const;
};
typedef slice<rule>		rulea;
extern char				string_buffer[256 * 32];
extern rulea			this_rules;
extern const char*		p;
void					errorv(const char* format, const char* format_param);
void					error(const char* format, ...);
void					parse(const char* source_code, const char* rule_id, rulea source);
void					parse(const char* source_code, const char* rule_id = 0);
void					setrules(rulea source);
void					skipws();
}