#include "bsreq.h"
#include "logparse.h"
#include "group.h"
#include "utg.h"

using namespace log;

BSDATAC(groupi, 256)
BSDATAC(groupvaluei, 2048)

BSMETA(groupi) = {
	BSREQ(id),
	{}};
BSMETA(groupvaluei) = {
	BSREQ(name), BSREQ(type),
	{}};

variant groupi::choose(variant group) {
	answers an;
	for(auto& e : bsdata<groupvaluei>()) {
		if(e.type != group)
			continue;
		an.add(&e, e.name);
	}
	char temp[260]; stringbuilder sb(temp);
	sb.add(group.getname());
	sb.add("?");
	return an.choose(temp);
}

static const char* read_name(const char* p, stringbuilder& result) {
	result.clear();
	while(*p) {
		char sym;
		if(*p == '\n' || *p == '\r' || *p == ',')
			break;
		else
			sym = *p++;
		switch(sym) {
		case 17: sym = '-'; break;
		case 0xA8: sym = 'Å'; break;
		case 0xB8: sym = 'å'; break;
		}
		result.add(sym);
	}
	return p;
}

static const char* read_identifier(const char* p, stringbuilder& result) {
	result.clear();
	return result.psidf(p);
}

static const char* read_variant(const char* p, stringbuilder& sb, variant& result) {
	p = read_identifier(p, sb);
	result = (const char*)sb.begin();
	if(!result)
		log::error(p, "Can't find variant `%1`", sb.begin());
	return p;
}

static bool ischax(unsigned char u) {
	return (u >= 'A' && u <= 'Z')
		|| (u >= 'a' && u <= 'z')
		|| (u >= '0' && u <= '9')
		|| (u >= 0xC0 && u <= 0xFF)
		|| (u >= 0x410 && u <= 0x44F);
}

static const char* read_line(const char* p, variant& type, stringbuilder& sb, bool& allowrun) {
	while(ischax(*p)) {
		auto pe = bsdata<groupvaluei>::add();
		pe->type = type;
		p = read_name(skipws(p), sb);
		pe->name = getstring(sb);
		p = skipws(p);
		if(*p == 13 || *p == 10 || *p == 0)
			break;
		if(*p != ',') {
			log::error(p, "Expected symbol `,`");
			allowrun = false;
			break;
		}
		p = skipwscr(p + 1);
	}
	return p;
}

void groupvaluei::read(const char* url) {
	auto p = log::read(url);
	if(!p)
		return;
	char temp[4096]; stringbuilder sb(temp);
	auto allowrun = true;
	while(allowrun && *p) {
		if(*p != '#') {
			log::error(p, "Expected symbol `#`");
			break;
		}
		variant id;
		p = read_variant(skipws(p + 1), sb, id);
		if(*p != 10 && *p != 13) {
			log::error(p, "Expected line feed");
			break;
		}
		p = read_line(skipwscr(p), id, sb, allowrun);
		p = skipwscr(p);
	}
	log::close();
}