#include "code_evalue.h"
#include "code_rule.h"
#include "strvar.h"
#include "stringbuilder.h"

using namespace code;

typedef adat<evalue> evaluea;
typedef adat<const char*> stringa;

static stringa		operations;
static evaluea		values;
static const char*	expected_pb;
static const char*	expected_id;

static void add(operation type, long value) {
	auto p = values.add();
	p->type = type;
	p->value = value;
}

static void string() {
	if(*p == '\"') {
		char temp[4096]; stringbuilder sb(temp);
		p = sb.psstr(p + 1, '\"');
		add(operation::Text, (long)szdup(temp));
		skipws();
	}
}

static void number() {
	long value = 0;
	auto p1 = p;
	p = stringbuilder::read(p, value);
	if(p1 != p) {
		add(operation::Number, value);
		skipws();
	}
}

static void identifier() {
	char temp[260]; stringbuilder sb(temp);
	auto p1 = p;
	p = sb.psidf(p);
	if(p1 != p) {
		add(operation::Identifier, (long)szdup(temp));
		skipws();
	}
}

operation getop(const char* p) {
	switch(p[0]) {
	case '+':
		return operation::Plus;
	case '-':
		return operation::Minus;
	case '>':
		if(p[1] == '=')
			return operation::GreaterEqual;
		return operation::Greater;
	case '<':
		if(p[1] == '=')
			return operation::LessEqual;
		return operation::Less;
	case '=':
		if(p[1] == '=')
			return operation::Equal;
		break;
	case '!':
		if(p[1] == '=')
			return operation::NotEqual;
		break;
	case '*':
		return operation::Mul;
	case '/':
		return operation::Div;
	}
	return operation::None;
}

static void binary_operation() {
	if(!last_token) {
		error("TokenEmpthy");
		return;
	}
	operations.add(last_token->id);
}

static void apply_binary_operation() {
	if(!operations) {
		error("OperationsEmpthy");
		return;
	}
	auto op = getop(operations.data[--operations.count]);
	auto& e1 = values.data[values.count - 1];
	auto& e2 = values.data[values.count - 2];
	e2.type = operation::Expression;
	e2.value = 0;
	values.count--;
}

static rule c2_lexer[] = {
	{"number", {}, number},
	{"string", {}, string},
	{"identifier", {}, identifier},
	{"if", {"if", "(", "%expression", ")", "%statement"}},
	{"while", {"while", "(", "%expression", ")", "%statement"}},
	{"block_statement", {"{", "?.%statement", "}"}},
	{"statement_part", {"^%if", "%while", "%block_statement", "%expression"}},
	{"statement", {"?%statement_part", ";"}},
	{"function_call", {"(", "?., %expression", ")"}},
	{"scored_expression", {"(", "%expression", ")"}},
	{"determinal", {"^%scored_expression", "%number", "%string", "%identifier"}},
	{"prefix", {"^++", "--", "\\&"}},
	{"indirection", {"\\.", "%identifier"}},
	{"scope", {"[", "%expression", "]"}},
	{"postfix", {"^++", "--", "%indirection", "%function_call", "%scope"}},
	{"unary", {"?%prefix", "%determinal", "?.%postfix"}},
	{"multiplication_op", {"^/", "*", "\\%"}, binary_operation},
	{"multiplication_op_end", {"%multiplication_op", "%unary"}, apply_binary_operation},
	{"multiplication", {"%.unary", "?.%multiplication_op_end"}},
	{"addiction_op", {"^+", "-"}, binary_operation},
	{"addiction_op_end", {"%addiction_op", "%multiplication"}, apply_binary_operation},
	{"addiction", {"%.multiplication", "?.%addiction_op_end"}},
	{"binary_op", {"^|", "&", "\\^"}, binary_operation},
	{"binary_op_end", {"%binary_op", "%addiction"}, apply_binary_operation},
	{"binary", {"%.addiction", "?.%binary_op_end"}},
	{"conditional_op", {"^>", "<", "<=", ">=", "==", "!="}, binary_operation},
	{"conditional_op_end", {"%conditional_op", "%binary"}, apply_binary_operation},
	{"conditional", {"%.binary", "?.%conditional_op_end"}},
	{"logical_op", {"^||", "&&"}, binary_operation},
	{"logical_op_end", {"%logical_op", "%conditional"}, apply_binary_operation},
	{"logical", {"%.conditional", "?.%logical_op_end"}},
	{"expression", {"%.logical"}},
};

void test_parser() {
	parse("10*2 + 7*4 / a[10] + function() + (i++)", c2_lexer, "expression");
}