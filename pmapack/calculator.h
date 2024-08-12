#pragma once

enum operation_s {
	Nop,
	Plus, Minus, Div, Mul, DivRest,
	BinaryOr, BinaryAnd, BinaryXor, Not, Neg,
	ShiftLeft, ShiftRight,
	Less, LessEqual, Greater, GreaterEqual, Equal, NotEqual,
	Or, And,
	Increment, Decrement, AdressOf, Dereference, Scope, Cast,
	Assign, If, While, Return, Switch, Case, Break, Continue, Default,
	Number, Text, Identifier, List, Initialization,
};
enum symbol_s {
	Void, i8, u8, i16, u16, i32, u32, i64, u64,
};
enum symbol_flag_s {
	Static, Public, Predefined, UseRead, UseWrite,
};
enum scope_s {
	TypeScope = -2, PointerScope = -3
};
struct symboli {
	int			ids; // string id, use string_name() to get text
	int			scope; // visibility scope (-2 for pointers, -3 for platform externs)
	int			type; // other symbol id
	int			value; // initialization value
	int			count; // 0 - default, 1+ for array
	unsigned	flags; // Some flags
	bool		is(symbol_flag_s v) const { return (flags & (1 << v)) != 0; }
	bool		isarray() const { return count >= 0; }
	bool		istype() const { return scope == TypeScope || scope == PointerScope; }
	bool		ispointer() const { return scope == PointerScope; }
	bool		ispredefined() const { return is(Predefined); }
	void		set(symbol_flag_s v) { flags |= (1 << v); }
};
struct asti {
	operation_s op;
	int			left;
	int			right;
};

typedef void(*calculator_fnprint)(const char* format, const char* format_param);
extern calculator_fnprint calculator_error_proc;

void calculator_file_parse(const char* url);
void calculator_parse(const char* code);
bool isterminal(operation_s v);
void symbol_ast(int sid, int value);
void symbol_count(int sid, int value);
void symbol_scope(int sid, int value);
void symbol_type(int sid, int value);

int ast_add(operation_s op, int left, int right);
int const_number(int ast);
int dereference(int type);
int findsym(int ids, int scope);
int reference(int type);
int symbol_ast(int sid);
int symbol_type(int sid);

const char* string_name(int sid);
