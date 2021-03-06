#include "crt.h"
#include "stringa.h"

#pragma once

namespace code {
typedef unsigned	pckh;
const unsigned		None = 0xFFFFFFFF;
enum class operation : unsigned char {
	None,
	Plus, Minus, Div, Mul, DivRest,
	BinaryOr, BinaryAnd, Binary??r, Not, Neg,
	ShiftLeft, ShiftRight,
	Less, LessEqual, Greater, GreaterEqual, Equal, NotEqual,
	Or, And,
	Increment, Decrement, AdressOf, Dereference, Scope,
	Assign, If, While,
	Number, Text, Identifier, Expression
};
enum : pckh {
	This, Systems, Modules, Pointers,
	Void, i8, u8, i16, u16, i32, u32, i64, u64,
	TypeLiteral, TypePointer,
};
struct evalue {
	operation			type;
	long				value;
	pckh				result;
};
struct ast {
	operation			type;
	pckh				right;
	pckh				left;
	constexpr bool isliteral() const { return type == operation::Text; }
	constexpr bool issymbol() const { return type == operation::Identifier; }
	constexpr bool isnumber() const { return type == operation::Number; }
};
struct symbol {
	pckh				id; // String identificator
	pckh				parent; // Parent symbol
	pckh				result; // Result symbol type of expression
	unsigned			index; // Position in source file
	unsigned			flags; // Various flags
	pckh				ast; // Abstract syntaxis tree index
	constexpr bool		is(int v) const { return (flags & (1 << v)) != 0; }
	constexpr bool		ispointer() const { return parent == Pointers; }
	constexpr void		set(int v) { flags |= 1 << v; }
};
class package {
	bool				serial(const char* url, bool write_mode);
public:
	stringa				strings;
	vector<symbol>		symbols;
	vector<ast>			asts;
	explicit operator bool() const { return strings.getcount() != 0; }
	pckh				add(const char* v) { return strings.add(v); }
	pckh				add(operation type, pckh left, pckh right);
	pckh				add(operation type, pckh left) { return add(type, left, 0); }
	pckh				add(pckh id, pckh parent, pckh result, unsigned flags, unsigned index);
	void				clear();
	void				create(const char* id);
	pckh				findast(operation type, pckh left, pckh right) const;
	pckh				findsym(pckh id, pckh parent) const;
	pckh				findsym(const char* id, pckh parent) const;
	pckh				findsym(pckh id, pckh parent, pckh type) const;
	const ast*			getast(pckh v) const { return asts.ptrs(v); }
	const char*			getstr(pckh v) const { return (const char*)strings.get(v); }
	symbol*				getsym(pckh v) const { return symbols.ptrs(v); }
	void				read(const char* url) { serial(url, false); }
	pckh				reference(pckh v);
	void				write(const char* url) { serial(url, true); }
};
}
