#include "code_rule.h"
#include "code_package.h"
#include "stringbuilder.h"

using namespace code;

rulea			code::rules;
fnerror			code::perror;
char			code::string_buffer[256 * 32];
const char*		code::p;
const char*		code::last_identifier;
const char*		code::last_string;
long			code::last_value;

static const char* example(const char* p) {
	static char temp[40]; stringbuilder sb(temp);
	sb.psstrlf(p);
	return temp;
}

void code::errorv(const char* p, const char* format, const char* format_param) {
	if(perror)
		perror(p, format, format_param);
}

void code::error(const char* format, ...) {
	errorv(p, format, xva_start(format));
}

static void comments() {
	if(p[0] == '/' && p[1] == '/') {
		p += 2;
		while(*p && *p != 10 && *p != 13)
			p++;
		p = skipspcr(p);
	}
}

void code::string() {
	if(*p == '\"') {
		stringbuilder sb(string_buffer);
		p = sb.psstr(p + 1, '\"');
		last_string = szdup(string_buffer);
		skipws();
	}
}

void code::number() {
	auto p1 = p;
	last_value = 0;
	p = stringbuilder::read(p, last_value);
	if(p1 != p) {
		skipws();
	}
}

void code::identifier() {
	auto p1 = p;
	stringbuilder sb(string_buffer);
	p = sb.psidf(p);
	if(p1 != p) {
		last_identifier = szdup(string_buffer);
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
	for(auto& e : rules) {
		if(strcmp(e.id, id) == 0)
			return &e;
	}
	return 0;
}

static void parse_token(const token& e);

static void parse_rule(const rule& v) {
	auto need_stop = false;
	auto p0 = p;
	for(auto& e : v.tokens) {
		if(!e)
			break;
		if(e.is(flag::Stop))
			need_stop = true;
		auto p1 = p;
		parse_token(e);
		if(p1 == p) {
			// Token did not work
			if(need_stop || e.is(flag::Condition)) // If tokens is optional continue executing
				continue;
			p = p0; // This rule is invalid, rollback all and exit
			return;
		}
		if(need_stop)
			break;
	}
	if(need_stop && p0 == p)
		return;
	if(v.determinal) {
		if((p0 != p) || !v.tokens[0])
			v.determinal(); // Only valid token execute proc
	}
}

static void parse_token(const token& e) {
	auto p0 = p;
	if(e.rule) {
		parse_rule(*e.rule);
		if(e.is(flag::Repeat)) {
			auto p2 = p0;
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
				parse_rule(*e.rule);
				if(p3 == p) {
					if(required)
						error("Expected %1", e.id);
				} else {
					if(!required) {
						if(e.is(flag::ComaSeparated))
							error("Expected symbol `,`");
						else if(e.is(flag::PointSeparated))
							error("Expected symbol `.`");
					}
				}
			}
		}
	} else {
		auto n = zlen(e.id);
		if(memcmp(p, e.id, n) != 0)
			return;
		last_token = &e;
		p += n;
		skipws();
	}
}

static void lazy_initialize() {
	for(auto& r : rules) {
		for(auto& e : r.tokens) {
			if(!e)
				break;
			if(e.rule)
				return; // All rules initialized
			if(e.is(flag::Variable)) {
				e.rule = find_rule(e.id);
				if(!e.rule)
					error("In rule `%1` not found token `%2`", r.id, e.id);
			}
		}
	}
}

void code::parse(const char* source_code, const char* rule_id) {
	if(!rule_id)
		rule_id = "global";
	auto pr = find_rule(rule_id);
	if(!pr) {
		error("Not found rule `%1`", rule_id);
		return;
	}
	p = source_code;
	skipws();
	while(*p) {
		auto pb = p;
		parse_rule(*pr);
		if(pb == p) {
			error("Can't find any rule when parse `%1`", example(pb));
			return;
		}
	}
}

void code::setrules(rulea source) {
	rules = source;
	lazy_initialize();
}

void code::parse(const char* source_code, const char* rule_id, rulea source) {
	setrules(source);
	parse(source_code, rule_id);
}
