#pragma once

namespace code {
enum class flag : unsigned char {
	Variable, Condition, Repeat, ComaSeparated, PointSeparated, Stop,
};
struct token {
	const char*			id;
	unsigned			flags;
	unsigned char		result;
	const struct rule*	rule;
	constexpr token() : id(0), flags(), rule(0), result(0xFF) {}
	constexpr token(const char* p) : id(p), flags(), rule(0), result(0xFF) {
		while(*p) {
			if(*p == '\\') {
				id = p + 1;
				break;
			} else if(p[0] == ',' && p[1] == ' ') {
				set(flag::ComaSeparated); p++;
			} else if(p[0] == '.' && p[1] == ' ') {
				set(flag::PointSeparated); p++;
			} else if(*p == '^')
				set(flag::Stop);
			else if(*p == '.')
				set(flag::Repeat);
			else if(*p == '%')
				set(flag::Variable);
			else if(*p == '?')
				set(flag::Condition);
			else if(*p >= '0' && *p <= '9')
				result = *p - '0';
			else {
				id = p;
				break;
			}
			p++;
		}
	}
	constexpr explicit operator bool() const { return id != 0; }
	constexpr bool		is(flag v) const { return (flags & (1 << (int)v)) != 0; }
	constexpr void		set(flag v) { flags |= 1 << (int)v; }
};
typedef token			tokena[16];
extern const token*		last_token;
}
