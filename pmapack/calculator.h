#pragma once

enum operation_s {
	None, Nop,
	Plus, Minus, Div, Mul, DivRest,
	BinaryOr, BinaryAnd, Binary’Ór, Not, Neg,
	ShiftLeft, ShiftRight,
	Less, LessEqual, Greater, GreaterEqual, Equal, NotEqual,
	Or, And,
	BooleanChoose, LeftIfTrue,
	Increment, Decrement, AdressOf, Dereference, Scope, Cast,
	Assign, If, While, Return, Switch, Case, Break, Default,
	Number, Text, Identifier, Statement
};
enum symbol_s {
	This, Systems, Modules, Pointers,
	Void, i8, u8, i16, u16, i32, u32, i64, u64,
};
enum symbol_flag_s {
	Constant, Static, Public,
};
struct symboli {
	int			ids; // string id, use string_name() to get text
	int			parent; // other symbol id
	int			scope; // visibility scope
	int			type; // other symbol id
	int			count; // array symbols count
	int			value; // initialization value
	unsigned	flags; // Some flags
	bool		is(symbol_flag_s v) const { return (flags & (1 << v)) != 0; }
	void		set(symbol_flag_s v) { flags |= (1 << v); }
};
struct asti {
	operation_s op;
	int			left;
	int			right;
};

void calculator_file_parse(const char* url);
void calculator_initialize();
void calculator_parse(const char* code);
void symbol_code_set(int sid, int value);

int ast_add(operation_s op, int left, int right);

const char* string_name(int sid);
