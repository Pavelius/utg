#include "code_rule.h"
#include "code_package.h"
#include "stringbuilder.h"

using namespace code;

typedef adat<evalue> evaluea;

fnevent			code::rule::papply;
rulea			code::this_rules;
char			code::string_buffer[256 * 32];
const char*		code::p;
static evaluea	values;
static int		last_index;
static int		last_value;

void code::errorv(const char* format, const char* format_param) {
}

void code::error(const char* format, ...) {
	errorv(getnm(format), xva_start(format));
}

static void addvalue(operation type, long value) {
	auto p = values.add();
	p->type = type;
	p->value = value;
}

static void comments() {
	if(p[0] == '/' && p[1] == '/') {
		p += 2;
		while(*p && *p != 10 && *p != 13)
			p++;
		p = skipspcr(p);
	}
}

void string() {
	if(*p == '\"') {
		stringbuilder sb(string_buffer);
		p = sb.psstr(p + 1, '\"');
		addvalue(operation::Text, (long)szdup(string_buffer));
		skipws();
	}
}

static void number() {
	auto p1 = p;
	long value = 0;
	p = stringbuilder::read(p, value);
	if(p1 != p) {
		addvalue(operation::Number, value);
		skipws();
	}
}

static void identifier() {
	auto p1 = p;
	stringbuilder sb(string_buffer);
	p = sb.psidf(p);
	if(p1 != p) {
		addvalue(operation::Identifier, (long)szdup(string_buffer));
		skipws();
	}
}

void code::skipws() {
	while(*p) {
		p = skipspcr(p);
		auto p1 = p;
		comments();
		if(p1 != p)
			continue;
		break;
	}
}

static rule* find_rule(const char* id) {
	for(auto& e : this_rules) {
		if(strcmp(e.id, id) == 0)
			return &e;
	}
	return 0;
}

static void parse_token(const token& e);

static void update_result(parser& e, const token& v) {
	if(v.result == 0xFF)
		return;
	const auto maximum = sizeof(e.param) / sizeof(e.param[0]);
	if(v.result < maximum)
		e.param[v.result] = last_value;
	else
		error("ErrorTokenResultIndex", v.result, maximum);
}

static void parse_rule(const rule& v) {
	parser ctx(p);
	auto need_stop = false;
	last_index = -1;
	for(auto& e : v.tokens) {
		if(!e)
			break;
		if(e.is(flag::Stop))
			need_stop = true;
		auto p1 = p;
		parse_token(e);
		if(p1 != p)
			update_result(ctx, e);
		last_index = &e - v.tokens;
		if(e.is(flag::Repeat)) {
			auto p2 = p1;
			while(p2 != p) {
				p2 = p;
				auto required = false;
				if(e.is(flag::ComaSeparated)) {
					if(p[0] == ',') {
						p++; skipws();
						required = true;
					}
				} else if(e.is(flag::PointSeparated)) {
					if(p[0] == '.') {
						p++; skipws();
						required = true;
					}
				}
				auto p3 = p;
				parse_token(e);
				if(p3 != p)
					update_result(ctx, e);
				if(p3 == p) {
					if(required)
						error("ErrorExpectedRule", e.id);
				} else {
					if(!required) {
						if(e.is(flag::ComaSeparated))
							error("ErrorExpectedSymbol", ",");
						else if(e.is(flag::PointSeparated))
							error("ErrorExpectedSymbol", ".");
					}
				}
			}
		}
		if(p1 == p) {
			// Token did not work
			if(need_stop || e.is(flag::Condition)) // If tokens is optional continue executing
				continue;
			if(ctx.p != p) {
				// Some of previous tokens work. Need rollback parser.
				p = ctx.p;
			}
			break;
		}
		if(need_stop)
			break;
	}
	if((p != ctx.p || !v.tokens[0])) {
		// Rule work
		v.apply(ctx);
	}
}

static void parse_token(const token& e) {
	if(e.rule)
		parse_rule(*e.rule);
	else {
		auto n = zlen(e.id);
		if(memcmp(p, e.id, n) != 0)
			return;
		last_token = &e;
		p += n;
		skipws();
	}
}

static void lazy_initialize() {
	for(auto& r : this_rules) {
		for(auto& e : r.tokens) {
			if(!e)
				break;
			if(e.rule)
				return; // All rules initialized
			if(e.is(flag::Variable)) {
				e.rule = find_rule(e.id);
				if(!e.rule)
					error("ErrorNotFoundRule", e.id);
			}
		}
	}
}

static void constant_number(operation type, evalue& e1, evalue& e2) {
	switch(type) {
	case operation::Plus: e1.value += e2.value; break;
	case operation::Minus: e1.value -= e2.value; break;
	case operation::Mul: e1.value *= e2.value; break;
	case operation::Div: e1.value /= e2.value; break;
	case operation::DivRest: e1.value %= e2.value; break;
	case operation::Scope: e1.value += e2.value * sizeof(int); break;
	default: break;
	}
}

static void loadvalue(evalue& e1) {
	if(e1.type == operation::Identifier) {
		e1.type = operation::Number;
		e1.value = 3;
	}
}

static void binary_operation(operation type, evalue& e1, evalue& e2) {
	loadvalue(e2);
	constant_number(type, e1, e2);
}

void rule::apply(parser& e) const {
	switch(type) {
	case Determinal:
		if(equal(id, "number"))
			number();
		else if(equal(id, "string"))
			string();
		else if(equal(id, "identifier"))
			identifier();
		break;
	case BinaryOperation:
		if(param)
			e.param[0] = param;
		if(!e.param[0])
			error("ErrorNotSpecifedBinaryOperation");
		else if(values.getcount() < 2)
			error("ErrorBinaryOperationOperandCount", values.getcount());
		else {
			binary_operation((operation)e.param[0], values.data[values.count - 2], values.data[values.count - 1]);
			values.count--;
		}
		break;
	case UnaryOperation:
		break;
	case SetValue:
		last_value = param;
		break;
	case SetValueRange:
		if(last_index == -1) {
			error("ErrorLastIndex", id);
			last_index = 0;
		}
		last_value = param + last_index;
		break;
	default:
		break;
	}
	if(papply)
		papply();
}

void code::parse(const char* source_code, const char* rule_id) {
	if(!rule_id)
		rule_id = "global";
	auto pr = find_rule(rule_id);
	if(!pr) {
		error("ErrorNotFoundRule", rule_id);
		return;
	}
	p = source_code;
	skipws();
	while(*p) {
		auto pb = p;
		parse_rule(*pr);
		if(pb == p) {
			error("ErrorParse", pb);
			return;
		}
	}
}

void code::setrules(rulea source) {
	this_rules = source;
	lazy_initialize();
}

void code::parse(const char* source_code, const char* rule_id, rulea source) {
	setrules(source);
	parse(source_code, rule_id);
}
