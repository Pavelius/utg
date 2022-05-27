#include "code_evalue.h"
#include "code_rule.h"
#include "stringbuilder.h"

using namespace code;

static rule c2_lexer[] = {
	{"number", {}, rule::Determinal},
	{"string", {}, rule::Determinal},
	{"identifier", {}, rule::Determinal},
	{"if", {"if", "(", "0%expression", ")", "1%statement"}},
	{"while", {"while", "(", "0%expression", ")", "1%statement"}},
	{"block_statement", {"{", "?.%statement", "}"}},
	{"statement_part", {"^%if", "%while", "%block_statement", "%expression"}},
	{"statement", {"?%statement_part", ";"}},
	{"function_call", {"(", "?., %expression", ")"}},
	{"scored_expression", {"(", "%expression", ")"}},
	{"determinal", {"^%scored_expression", "%number", "%string", "%identifier"}},
	{"prefix", {"^++", "--", "\\&"}, rule::SetValueRange, (int)operation::Increment},
	{"indirection", {"\\.", "%identifier"}},
	{"scope", {"[", "%expression", "]"}, rule::BinaryOperation, (int)operation::Scope},
	{"postfix", {"^++", "--", "%indirection", "%function_call", "%scope"}},
	{"unary", {"?%prefix", "%determinal", "?.%postfix"}},
	{"multiplication_op", {"^/", "*", "\\%"}, rule::SetValueRange, (int)operation::Div},
	{"multiplication_op_end", {"0%multiplication_op", "%unary"}, rule::BinaryOperation},
	{"multiplication", {"%.unary", "?.%multiplication_op_end"}},
	{"addiction_op", {"^+", "-"}, rule::SetValueRange, (int)operation::Plus},
	{"addiction_op_end", {"0%addiction_op", "%multiplication"}, rule::BinaryOperation},
	{"addiction", {"%.multiplication", "?.%addiction_op_end"}},
	{"binary_op", {"^|", "&", "\\^"}, rule::SetValueRange, (int)operation::BinaryOr},
	{"binary_op_end", {"0%binary_op", "%addiction"}, rule::BinaryOperation},
	{"binary", {"%.addiction", "?.%binary_op_end"}},
	{"conditional_op", {"^<", "<=", ">", ">=", "==", "!="}, rule::SetValueRange, (int)operation::Less},
	{"conditional_op_end", {"0%conditional_op", "%binary"}, rule::BinaryOperation},
	{"conditional", {"%.binary", "?.%conditional_op_end"}},
	{"logical_op", {"^||", "&&"}, rule::SetValueRange, (int)operation::Or},
	{"logical_op_end", {"0%logical_op", "%conditional"}, rule::BinaryOperation},
	{"logical", {"%.conditional", "?.%logical_op_end"}},
	{"expression", {"%.logical"}},
};

void test_parser() {
	parse("10*2 + 7*4 / a[10] + function() + (i++)", "expression", c2_lexer);
}