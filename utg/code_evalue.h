#pragma once

namespace code {
enum class operation : unsigned char {
	None,
	Plus, Minus, Div, Mul, DivRest,
	BinaryOr, BinaryAnd, Binary��r, Not, Neg,
	ShiftLeft, ShiftRight,
	Less, LessEqual, Greater, GreaterEqual, Equal, NotEqual,
	Or, And,
	Assign, If, While,
	Number, Text, Identifier, Expression
};
struct evalue {
	operation		type;
	long			value;
};
}
