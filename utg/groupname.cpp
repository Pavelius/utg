#include "collection.h"
#include "groupname.h"
#include "log.h"
#include "stringbuilder.h"

using namespace log;

BSDATAC(groupname, 2048);

const groupname* random_group_name(const char* parent) {
	collection<groupname> source;
	for(auto& e : bsdata<groupname>()) {
		if(szpmatch(e.parent, parent))
			source.add(&e);
	}
	return source.random();
}

short unsigned random_group_namei(const char* parent) {
	auto pn = random_group_name(parent);
	if(!pn)
		return 0xFFFF;
	return pn - bsdata<groupname>::elements;
}

static const char* read_line(const char* p, const char* parent, stringbuilder& sb) {
	while(ischa(*p)) {
		auto pe = bsdata<groupname>::add();
		memset(pe, 0, sizeof(*pe));
		sb.clear();
		pe->parent = parent;
		p = sb.psparam(skipws(p));
		pe->name = szdup(sb);
		p = skipws(p);
		if(*p == 13 || *p == 10 || *p == 0)
			break;
		if(!checksym(p, ','))
			break;
		p = skipwscr(p + 1);
	}
	return p;
}

void read_group_names(const char* url) {
	auto p = log::read(url);
	if(!p)
		return;
	char temp[4096]; stringbuilder sb(temp);
	allowparse = true;
	while(allowparse && *p) {
		if(!checksym(p, '#'))
			break;
		sb.clear();
		p = sb.psidf(skipws(p + 1));
		if(!checksym(p, '\n'))
			break;
		p = read_line(skipwscr(p), szdup(temp), sb);
		p = skipwscr(p);
	}
	close();
}