#include "speech.h"
#include "logparse.h"

using namespace log;

BSDATAC(speech, 1024)

bool speech::match(variant key, const variants& values) {
	for(auto v : values) {
		if(!v)
			break;
		if(v == key)
			return true;
	}
	return false;
}

bool speech::match(const slice<variant>& keys, const variants& values) {
	for(auto v : keys) {
		if(!match(v, values))
			return false;
	}
	return true;
}

speech* speech::find(const slice<variant>& keys) {
	for(auto& e : bsdata<speech>()) {
		if(match(keys, e.keys))
			return &e;
	}
	return 0;
}

static const char* read_line(const char* p, stringbuilder& sb) {
	sb.clear();
	return sb.psstrlf(p);
}

static const char* read_conditions(const char* p, stringbuilder& sb, variant* pb, const variant* pe) {
	auto count = pe - pb;
	variant v;
	while(ischa(p[0])) {
		p = readval(p, sb, v);
		if(pb >= pe)
			log::error(p, "Too many conditions when save variant %1 (only %2i allowed)", v.getid(), count);
		else
			*pb++ = v;
		p = skipws(p);
	}
	return p;
}

void speech::read(const char* url) {
	auto p = log::read(url);
	if(!p)
		return;
	char temp[4096]; stringbuilder sb(temp);
	allowparse = true;
	while(allowparse && *p) {
		if(!checksym(p, '#'))
			break;
		auto ps = bsdata<speech>::add();
		ps->name = "Error";
		p = read_conditions(skipws(p + 1), sb, ps->keys, ps->keys + sizeof(ps->keys) / sizeof(ps->keys[0]));
		if(!checksym(p, '\n'))
			break;
		p = read_line(skipwscr(p), sb);
		p = skipwscr(p);
		ps->name = szdup(temp);
	}
	log::close();
}