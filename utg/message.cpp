#include "log.h"
#include "message.h"

using namespace log;

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

static const char* read_conditions(const char* p, stringbuilder& sb, messagei* ps) {
	while(true) {
		if(isnum(p[0])) {
			p = psnum(p, ps->value);
			p = skipws(p);
		} else if(ischa(p[0])) {
			p = sb.psidf(p);
			auto pn = sb.begin();
			if(equal(pn, "Random"))
				ps->set(messagei::Random);
			else if(equal(pn, "Unique"))
				ps->set(messagei::Unique);
			else {
				variant v = (const char*)sb.begin();
				if(!v)
					log::errorp(p, "Can't find variant `%1`", sb.begin());
				else if(!ps->add(v))
					log::errorp(p, "Too many conditions when save variant %1 (only %2i allowed)", v.getid(), sizeof(ps->conditions) / sizeof(ps->conditions[0]));
			}
			p = skipws(p);
		} else
			break;
	}
	return p;
}

static const char* read_part(const char* p, variant type, stringbuilder& sb) {
	while(allowparse && *p && *p != '#') {
		auto pe = bsdata<messagei>::add(); pe->clear();
		pe->type = type;
		p = read_conditions(skipwscr(p), sb, pe);
		if(!checksym(p, ':'))
			break;
		p = sb.psstrlf(skipws(p + 1));
		pe->text = szdup(sb);
		p = skipwscr(p);
	}
	return p;
}

void messagei::read(const char* url) {
	auto p = log::read(url);
	if(!p)
		return;
	char temp[4096]; stringbuilder sb(temp);
	allowparse = true;
	while(allowparse && *p) {
		if(!checksym(p, '#'))
			break;
		variant type;
		// p = readval(skipws(p + 1), sb, type);
		p = skipws(p);
		if(!checksym(p, '\n'))
			break;
		p = read_part(p, type, sb);
	}
	log::close();
}