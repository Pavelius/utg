#include "bsreq.h"
#include "code_lexer.h"
#include "variant.h"
#include "viewpackage.h"
#include "widget.h"

using namespace code;

BSDATAC(lexer, 32)
BSDATAC(token, 512)
BSDATAC(viewpackage, 4096)

BSMETA(lexer) = {
	BSREQ(id),
	BSREQ(files),
	{}};
BSMETA(operationi) = {
	BSREQ(id),
	{}};
BSMETA(rule) = {
	BSREQ(id),
	BSREQ(tokens),
	{}};
BSMETA(token) = {
	BSREQ(id),
	{}};

BSDATA(varianti) = {
	{"NoVariant"},
	{"Lexer", VAR(lexer, 1), 0, 0, 0, 0, lexer::read},
	{"Rule", VAR(rule, 1)},
	{"Widget", VAR(widget, 1)},
};
BSDATAF(varianti)