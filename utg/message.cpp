#include "log.h"
#include "message.h"

using namespace log::parse;

BSDATAC(messagei, 1024)

void messagei::clear() {
	memset(this, 0, sizeof(*this));
}

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
		case -72: sym = 'å'; break;
		case -105: case 17: sym = '-'; break;
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

static const char* read_conditions(const char* p, stringbuilder& sb, messagei* ps) {
	while(true) {
		if(isnum(p[0])) {
			p = sb.read(p, ps->value);
			p = skipws(p);
		} else if(ischa(p[0])) {
			p = read_identifier(p, sb);
			auto pn = sb.begin();
			if(equal(pn, "Random"))
				ps->set(messagei::Random);
			else if(equal(pn, "Unique"))
				ps->set(messagei::Unique);
			else {
				variant v = (const char*)sb.begin();
				if(!v)
					log::error(p, "Can't find variant `%1`", sb.begin());
				else if(!ps->add(v))
					log::error(p, "Too many conditions when save variant %1 (only %2i allowed)", v.getid(), sizeof(ps->conditions) / sizeof(ps->conditions[0]));
			}
			p = skipws(p);
		} else
			break;
	}
	return p;
}

static const char* read_part(const char* p, variant type, stringbuilder& sb, bool& allowrun) {
	while(allowrun && *p && *p != '#') {
		auto pe = bsdata<messagei>::add(); pe->clear();
		pe->type = type;
		p = read_conditions(skipwscr(p), sb, pe);
		if(*p != ':') {
			log::error(p, "Expected symbol `:`");
			allowrun = false;
			break;
		}
		p = read_string(skipws(p + 1), sb);
		pe->text = getstring(sb);
		p = skipwscr(p);
	}
	return p;
}

void messagei::read(const char* url) {
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
		variant type;
		p = read_variant(skipws(p + 1), sb, type);
		p = skipws(p);
		if(*p != 13 && *p != 10) {
			log::error(p, "Expected symbol line feed");
			break;
		}
		p = read_part(p, type, sb, allowrun);
	}
	log::close();
}