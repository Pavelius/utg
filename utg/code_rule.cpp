#include "code_rule.h"
#include "stringbuilder.h"

using namespace code;

rulea			code::this_rules;
const char*		code::p;

void code::errorv(const char* format, const char* format_param) {
}

void code::error(const char* format, ...) {
	errorv(getnm(format), xva_start(format));
}

static void comments() {
	if(p[0] == '/' && p[1] == '/') {
		p += 2;
		while(*p && *p != 10 && *p != 13)
			p++;
		p = skipspcr(p);
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

static void parse_rule(const rule& v) {
	auto pb = p;
	auto need_stop = false;
	for(auto& e : v.tokens) {
		if(!e)
			break;
		if(e.is(flag::Stop))
			need_stop = true;
		auto p1 = p;
		parse_token(e);
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
				if(p3 == p) {
					if(required)
						error("ExpectedRule", e.id);
				} else {
					if(!required) {
						if(e.is(flag::ComaSeparated))
							error("ExpectedSymbol", ",");
						else if(e.is(flag::PointSeparated))
							error("ExpectedSymbol", ".");
					}
				}
			}
		}
		if(p1 == p) { // Case when token did not work
			if(need_stop || e.is(flag::Condition)) // If tokens is optional continue executing
				continue;
			if(pb != p) {
				// Some of previous tokens match.
				// Need rollback parser.
				p = pb;
			}
			break;
		}
		if(need_stop)
			break;
	}
	if(v.special && (p != pb || !v.tokens[0]))
		v.special();
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
					error("NotFoundRule", e.id);
			}
		}
	}
}

void code::parse(const char* source_code, rulea source, const char* rule_id) {
	if(!rule_id)
		rule_id = "global";
	this_rules = source;
	lazy_initialize();
	auto pr = find_rule(rule_id);
	if(!pr) {
		error("NotFoundRule", rule_id);
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