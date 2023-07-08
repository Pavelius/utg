#include "bsreq.h"
#include "code_lexer.h"
#include "variant.h"
#include "viewpackage.h"
#include "widget.h"

using namespace code;

BSDATAC(lexer, 32)
BSDATAC(rule, 512)
BSDATAC(ruleop, 512)
BSDATAC(token, 512)
BSDATAC(viewpackage, 512)

BSMETA(lexer) = {
	BSREQ(id),
	BSREQ(files),
	BSREQ(rules),
	BSREQ(binary),
	BSREQ(unary),
	BSREQ(postfix),
	{}};
BSMETA(operationi) = {
	BSREQ(id),
	{}};
BSMETA(rule) = {
	BSREQ(id),
	BSREQ(tokens),
	{}};
BSMETA(ruleop) = {
	BSREQ(id),
	BSENM(value, operationi),
	{}};
BSMETA(token) = {
	BSREQ(id),
	{}};

BSDATA(varianti) = {
	{"NoVariant"},
	{"Lexer", VAR(lexer, 1)},
	{"Rule", VAR(rule, 1)},
	{"RuleOperation", VAR(ruleop, 2)},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)