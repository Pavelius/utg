#include "compiler.h"
#include "logparse.h"
#include "stringa.h"

#define EMPTHY 0xFFFFFFFF

using namespace log;
using namespace compiler;

BSDATAC(evaluei, 256 * 256)

static const char*	p;
static stringa		literals;
static evaluei		stack[32];
static evaluei*		pv = stack;
static char			name[260];
static unsigned		last_exression;

void evaluei::clear() {
	op = NOP;
	p1 = p2 = EMPTHY;
}

static unsigned find_exist(const evaluei& v1) {
	for(auto& v : bsdata<evaluei>()) {
		if(v == v1)
			return &v - bsdata<evaluei>::elements;
	}
	return EMPTHY;
}

unsigned evaluei::instance() const {
	auto i = find_exist(*this);
	if(i == EMPTHY) {
		i = bsdata<evaluei>::source.getcount();
		bsdata<evaluei>::source.add(this);
	}
	return i;
}

static bool isdeterminal(op_s v) {
	switch(v) {
	case Literal: case Number: case Attribute:
		return true;
	default:
		return false;
	}
}

static bool isconstant(op_s v) {
	return isdeterminal(v);
}

static bool isconstant(const evaluei& v) {
	return isdeterminal(v.op);
}

static void skipws() {
	p = skipws(p);
}

static bool skip(const char* value) {
	auto ps = p;
	while(*value) {
		if(*p != *value) {
			p = ps;
			error("Expected `%1`", value);
			return false;
		}
	}
	skipws();
	return true;
}

static void skip(int count) {
	p += count;
	skipws();
}

static void pushval(op_s op, unsigned p1, unsigned p2 = 0) {
	pv->op = op;
	pv->p1 = p1;
	pv->p2 = p2;
	pv++;
}

static void pushval(op_s op) {
}

static void popv() {
	if(pv > stack)
		pv--;
}

static unsigned getinstance() {
	return pv->instance();
}

static void pushval(const char* value) {
	pushval(Literal, literals.add(value));
}

static void pushval(int value) {
	pushval(Number, value);
}

static bool binary_optimization(op_s v, evaluei& e1, evaluei& e2) {
	if(e1.op == Number && e2.op == Number) {
		switch(v) {
		case Add: e1.p1 += e2.p1; break;
		case Sub: e1.p1 -= e2.p1; break;
		case Mul: e1.p1 *= e2.p1; break;
		case Div:
			if(!e2.p1)
				error(p, "Division by zero");
			else
				e1.p1 /= e2.p1;
			break;
		case Lesser: e1.p1 = e1.p1 < e2.p1; break;
		case LesserEqual: e1.p1 = e1.p1 <= e2.p1; break;
		case Greater: e1.p1 = e1.p1 > e2.p1; break;
		case GreaterEqual: e1.p1 = e1.p1 >= e2.p1; break;
		case Equal: e1.p1 = e1.p1 == e2.p1; break;
		case NotEqual: e1.p1 = e1.p1 != e2.p1; break;
		default: return false;
		}
		return true;
	}
	return false;
}

static bool unary_optimization(op_s v, evaluei& e1) {
	if(e1.op == Number) {
		switch(v) {
		case Not: e1.p1 = !e1.p1; break;
		default: return false;
		}
		return true;
	}
	return false;
}

static void unary(op_s v) {
	if(unary_optimization(v, pv[0]))
		return;
	auto p1 = pv[0].instance();
	pv->op = v;
	pv->p1 = p1;
	pv->p2 = 0;
}

static void binary(op_s v) {
	if(pv <= stack) {
		error(p, "Binary operation stack corrupt");
		return;
	}
	if(binary_optimization(v, pv[-1], pv[0])) {
		pv--;
		return;
	}
	auto p1 = pv[-1].instance();
	auto p2 = pv[0].instance();
	pv--;
	pv->op = v;
	pv->p1 = p1;
	pv->p2 = p2;
}

static void pushidf() {
}

static void expression();

static char next_literal() {
	while(*p) {
		if(*p != '\\')
			return *p++;
		p++;
		switch(*p++) {
		case 0: return 0;
		case 'n': return '\n';
		case 't': return '\t';
		case 'r': return '\r';
		case '\\': return '\\';
		case '\'': return '\'';
		case '\"': return '\"';
		case '\n':
			if(p[0] == '\r')
				p++;
			break;
		case '\r':
			if(p[0] == '\n')
				p++;
			break;
		default: break;
		}
	}
	return 0;
}

void literal() {
	if(*p == '\"') {
		char temp[1024]; stringbuilder sb(temp);
		p++;
		while(*p && *p != '\"')
			sb.add(next_literal());
		if(*p == '\"') {
			p++;
			skipws();
		}
		pushval(temp);
	}
}

static void number() {
	auto value = 0;
	if(!isnum(*p) && !(p[0] == '-' && isnum(p[1])))
		return;
	if(p[0] == '0') {
		if(p[1] == 'x') {
			p += 2;
			while(true) {
				char s = *p;
				if(s >= 'a' && s <= 'f')
					s = s - 'a' + 10;
				else if(s >= 'A' && s <= 'F')
					s = s - 'A' + 10;
				else if(s >= '0' && s <= '9')
					s = s - '0';
				else
					break;
				value = value * 16 + s;
				p++;
			}
		} else {
			while(*p >= '0' && *p <= '7') {
				value = value * 8 + *p - '0';
				p++;
			}
		}
	} else {
		while(*p >= '0' && *p <= '9') {
			value = value * 10 + *p - '0';
			p++;
		}
	}
	if(*p == 'f' || *p == 'e')
		p++;
	skipws();
}

static void identifier() {
	if(!ischa(*p))
		return;
	stringbuilder sb(name); name[0] = 0;
	p = sb.psidf(p);
	skipws();
}

static void postfix() {
	if(*p == '(') {
		while(*p != ')') {
			expression();
			skip(",");
		}
	}
}

static void unary() {
	switch(*p) {
	case '-':
		skip(1);
		unary();
		pushval(Neg);
		break;
	case '+':
		skip(1);
		unary();
		break;
	case '(':
		skip(1);
		expression();
		skip(")");
		break;
	default:
		if(ischa(*p)) {
			identifier();
			pushidf();
		} else if(isnum(*p))
			number();
		break;
	}
	postfix();
}

static void binary_add() {
	unary();
	while(p[0] == '+' || p[0] == '-') {
		auto op = Add;
		if(p[0] == '-')
			op = Sub;
		unary();
		binary(op);
	}
}

static void binary_mul() {
	binary_add();
	while(p[0] == '*' || p[0] == '/' || p[0] == '%') {
		auto op = Mul;
		if(p[0] == '/')
			op = Div;
		else
			op = DivRest;
		binary_add();
		binary(op);
	}
}

static void binary_compare() {
	unary();
	while(p[0] == '>' || p[1] == '<' || (p[0] == '!' && p[1] == '=') || (p[0] == '=' && p[1] == '=')) {
		auto op = Lesser;
		if(p[0] == '>' && p[1] == '=') {
			skip(2);
			op = GreaterEqual;
		} else if(p[0] == '<' && p[1] == '=') {
			skip(2);
			op = LesserEqual;
		} else if(p[0] == '!' && p[1] == '=') {
			skip(2);
			op = NotEqual;
		} else if(p[0] == '=' && p[1] == '=') {
			skip(2);
			op = Equal;
		} else if(p[0] == '>') {
			skip(1);
			op = Greater;
		} else
			skip(1);
		unary();
		binary(op);
	}
}

static void binary_and() {
	binary_compare();
	while(p[0] == '&' && p[1] == '&') {
		binary_compare();
		binary(And);
	}
}

static void binary_or() {
	binary_and();
	while(p[0] == '|' && p[1] == '|') {
		binary_and();
		binary(Or);
	}
}

static void expression() {
	binary_or();
}

int compiler::compile(const char* source_code) {
	return 0;
}