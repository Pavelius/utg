#include "charname.h"
#include "log.h"

using namespace log::parse;

static const unsigned max_size = 512;
BSDATAC(charname, max_size)

bool charname::match(variant v) const {
	for(auto e : conditions) {
		if(e == v)
			return true;
	}
	return false;
}

bool charname::match(const slice<variant>& source) const {
	for(auto e : source) {
		if(!e)
			break;
		if(!match(e))
			return false;
	}
	return true;
}

const char* charname::getname(unsigned short v) {
	if(v == 0xFFFF)
		return "No name";
	return bsdata<charname>::elements[v].name;
}

unsigned charname::select(short unsigned* pb, short unsigned* pe, const slice<variant>& source) {
	auto ps = pb;
	for(auto& e : bsdata<charname>()) {
		if(!e.match(source))
			continue;
		if(ps < pe)
			*ps++ = &e - bsdata<charname>::elements;
	}
	return ps - pb;
}

short unsigned charname::random(const slice<variant>& source) {
	short unsigned temp[max_size];
	auto count = select(temp, temp + sizeof(temp) / sizeof(temp[0]), source);
	if(count)
		return temp[rand() % count];
	return 0xFFFF;
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

static const char* getstring(stringbuilder& sb) {
	auto p = sb.begin();
	if(!p[0])
		return 0;
	return szdup(p);
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
		|| (u >= 0xC0 && u <= 0xFF)
		|| (u >= 0x410 && u <= 0x44F);
}

static const char* read_line(const char* p, variant* conditions, stringbuilder& sb, bool& allowrun) {
	while(ischax(*p)) {
		auto pe = bsdata<charname>::add();
		memset(pe, 0, sizeof(*pe));
		memcpy(pe->conditions, conditions, sizeof(pe->conditions));
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

static const char* read_conditions(const char* p, stringbuilder& sb, variant* pb, const variant* pe) {
	auto count = pe - pb;
	while(ischa(p[0])) {
		p = read_identifier(p, sb);
		variant v = (const char*)sb.begin();
		if(!v)
			log::error(p, "Can't find variant `%1`", sb.begin());
		else if(pb >= pe)
			log::error(p, "Too many conditions when save variant %1 (only %2i allowed)", v.getid(), count);
		else
			*pb++ = v;
		p = skipws(p);
	}
	return p;
}

void charname::read(const char* url) {
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
		variant conditions[4] = {};
		p = read_conditions(skipws(p + 1), sb, conditions, conditions + sizeof(conditions) / sizeof(conditions[0]));
		if(*p != 10 && *p != 13) {
			log::error(p, "Expected line feed");
			break;
		}
		p = read_line(skipwscr(p), conditions, sb, allowrun);
		p = skipwscr(p);
	}
	log::close();
}