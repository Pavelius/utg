#include "collection.h"
#include "groupname.h"
#include "log.h"
#include "logparse.h"

using namespace log;

BSDATAC(groupname, 2048);

const groupname* groupname::random(const char* parent) {
	collection<groupname> source;
	for(auto& e : bsdata<groupname>()) {
		if(equal(e.parent, parent))
			source.add(&e);
	}
	return source.random();
}

short unsigned groupname::randomid(const char* parent) {
	auto pn = random(parent);
	if(!pn)
		return 0xFFFF;
	return pn - bsdata<groupname>::elements;
}

static bool ischax(unsigned char u) {
	return (u >= 'A' && u <= 'Z')
		|| (u >= 'a' && u <= 'z')
		|| (u >= 0xC0 && u <= 0xFF)
		|| (u >= 0x410 && u <= 0x44F);
}

static const char* read_line(const char* p, const char* parent, stringbuilder& sb) {
	while(ischax(*p)) {
		auto pe = bsdata<groupname>::add();
		memset(pe, 0, sizeof(*pe));
		pe->parent = parent;
		p = readname(skipws(p), sb);
		pe->name = getstring(sb);
		p = skipws(p);
		if(*p == 13 || *p == 10 || *p == 0)
			break;
		if(!checksym(p, ','))
			break;
		p = skipwscr(p + 1);
	}
	return p;
}

void groupname::read(const char* url) {
	auto p = log::read(url);
	if(!p)
		return;
	char temp[4096]; stringbuilder sb(temp);
	allowparse = true;
	while(allowparse && *p) {
		if(!checksym(p, '#'))
			break;
		p = readname(skipws(p + 1), sb);
		if(!checksym(p, '\n'))
			break;
		p = read_line(skipwscr(p), szdup(temp), sb);
		p = skipwscr(p);
	}
	log::close();
}