#include "code_package.h"

using namespace code;

BSDATA(operationi) = {
	{"OpNone"},
	{"BinaryPlus"},
	{"BinaryMinus"},
	{"BinaryDiv"},
	{"BinaryMul"},
	{"BinaryDivRest"},
	{"BinaryOr"},
	{"BinaryAnd"},
	{"BinaryXor"},
	{"UnaryNot"},
	{"UnaryNeg"},
	{"ShiftLeft"},
	{"ShiftRight"},
	{"ConditionLess"},
	{"ConditionLessEqual"},
	{"ConditionGreater"},
	{"ConditionGreaterEqual"},
	{"ConditionEqual"},
	{"ConditionNotEqual"},
	{"ConditionOr"},
	{"ConditionAnd"},
};
assert_enum(operationi, operation::And)