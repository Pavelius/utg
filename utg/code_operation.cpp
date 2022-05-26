#include "crt.h"
#include "code_evalue.h"

using namespace code;

BSDATA(operationi) = {
	{""},
	{"+", 2},
	{"-", 2},
	{"*", 2},
	{"/", 2},
	{"%", 2},
	{"&", 2},
	{"|", 2},
	{"!", 1},
	{"-", 1},
	{"<<", 2},
	{">>", 2},
	{"<", 2},
	{"<=", 2},
	{">", 2},
	{">=", 2},
	{"==", 2},
	{"!=", 2},
	{"=", 2},
	{"if"},
	{"while"},
	{"number"},
	{"text"},
	{"identifier"},
	{"expression"},
};
assert_enum(operationi, operation::Expression)