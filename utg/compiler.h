#pragma once

namespace compiler {
enum op_s : unsigned char {
	NOP,
	Neg, Not,
	Lesser, LesserEqual, Greater, GreaterEqual, NotEqual, Equal, And, Or,
	Add, Sub, Mul, Div, DivRest, Call, Expression,
	Literal, Number, Attribute
};
struct evaluei {
	op_s		op;
	unsigned	p1, p2;
	void		clear();
	unsigned	instance() const;
	constexpr bool operator==(const evaluei& v) const { return op == v.op && p1 == v.p1 && p2 == v.p2; }
};
int	compile(const char* source_code);
};