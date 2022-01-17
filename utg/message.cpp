#include "log.h"
#include "message.h"

BSDATAC(messagei, 1024)

static bool matchv(variant v, const messagei::variants& source) {
	for(auto e : source) {
		if(e == v)
			return true;
	}
	return false;
}

bool messagei::match(const messagei::variants& source) const {
	for(auto v : conditions) {
		if(!v)
			return true;
		if(!matchv(v, source))
			return false;
	}
	return true;
}

bool messagei::match(variant vs) const {
	for(auto v : conditions) {
		if(!v)
			return false;
		if(v == vs)
			return true;
	}
	return false;
}

bool messagei::add(variant v) {
	for(auto& e : conditions) {
		if(e)
			continue;
		e = v;
		return true;
	}
	return false;
}

const messagei* messagei::find(variants source) {
	for(auto& e : bsdata<messagei>()) {
		if(!e.match(source))
			continue;
		return &e;
	}
	return 0;
}

static const char* read_string(const char* p, stringbuilder& result) {
	result.clear();
	while(*p) {
		char sym;
		if(*p == '\n' || *p == '\r')
			break;
		else
			sym = *p++;
		switch(sym) {
		case 17: sym = '-'; break;
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

static const char* read_conditions(const char* p, stringbuilder& sb, messagei* ps) {
	while(true) {
		if(isnum(p[0])) {
			p = sb.read(p, ps->value);
			p = skipsp(p);
		} else if(ischa(p[0])) {
			p = read_identifier(p, sb);
			p = skipsp(p);
			variant v = (const char*)sb.begin();
			if(!v) {
				log::error(p, "Can't find variant `%1`", sb.begin());
				continue;
			}
			if(!ps->add(v))
				log::error(p, "Too many conditions when save variant %1 (only %1i allowed)", v.getid(), sizeof(ps->conditions) / sizeof(ps->conditions[0]));
		} else
			break;
	}
	return p;
}

void messagei::read(const char* url) {
	auto p = log::read(url);
	if(!p)
		return;
	variants resolves;
	char temp[4096]; stringbuilder sb(temp);
	while(*p) {
		auto pe = bsdata<messagei>::add();
		p = read_conditions(skipspcr(p), sb, pe);
		if(*p != ':') {
			log::error(p, "Expected symbol `:`");
			break;
		}
		p = read_string(skipsp(p + 1), sb);
		pe->text = getstring(sb);
	}
	log::close();
}