#pragma once

namespace code {
enum class operation : unsigned char {
	None,
	Plus, Minus, Mul, Div, DivRest,
	And, Or, Not, Neg, ShiftRight, ShiftLeft,
	Less, LessEqual, Greater, GreaterEqual, Equal, NotEqual,
	Assign, If, While,
	Number, Text, Identifier, Expression
};
struct operationi {
	const char*		id;
	int				operands;
};
struct evalue {
	operation		type;
	long			value;
};
}
