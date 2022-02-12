#include "answers.h"
#include "condition.h"
#include "logparse.h"
#include "quest.h"

BSDATAC(quest, 2048)

int quest::prop_image;
int quest::prop_header;
const quest* quest::last;

using namespace log;

static bool isanswer(const char* p) {
	return isnum(*p);
}

static bool isevent(const char* p) {
	return p[0] == '#' && isnum(p[1]);
}

static const char* read_string(const char* p, stringbuilder& result) {
	result.clear();
	if(p[0] == '#' || isnum(p[0]))
		return p;
	while(*p) {
		if(*p == '\n' || *p == '\r') {
			p = skipwscr(p);
			if(p[0] == '#' || isanswer(p))
				break;
			result.addch('\n');
		} else
			result.addch(*p++);
	}
	return p;
}

static void add(variants& e, variant v) {
	if(!e.count)
		e.start = bsdata<variant>::source.getcount();
	auto p = (variant*)bsdata<variant>::source.add();
	*p = v;
	e.count++;
}

static const char* read_params(const char* p, short& v) {
	if(!checksym(p, '('))
		return p;
	p = skipws(p + 1);
	p = stringbuilder::read(p, v);
	p = skipws(p);
	if(!checksym(p, ')'))
		return p;
	p = skipws(p + 1);
	return p;
}

static const char* read_params(const char* p, stringbuilder& result) {
	if(!checksym(p, '('))
		return p;
	p = skipws(p + 1);
	if(p[0] == '\"') {
		result.clear();
		p = result.psstr(p + 1, p[0]);
	} else
		p = readidn(p, result);
	p = skipws(p);
	if(!checksym(p, ')'))
		return p;
	p = skipws(p + 1);
	return p;
}

static const char* read_variants(const char* p, stringbuilder& result, variants& source, quest* pe) {
	while(allowparse && ischa(*p)) {
		p = readidn(p, result);
		p = skipws(p);
		auto pn = result.begin();
		int bonus; p = readbon(p, bonus);
		p = skipws(p);
		variant v = (const char*)result.begin();
		if(!v)
			log::error(p, "Can't find variant `%1`", result.begin());
		else
			v.counter = bonus;
		add(source, v);
	}
	return p;
}

static const char* read_event(const char* p, short& parent, stringbuilder& sb) {
	if(!allowparse)
		return p;
	p = stringbuilder::read(skipws(p), parent);
	auto pe = bsdata<quest>::add(); pe->clear();
	pe->index = parent;
	pe->next = -1;
	p = propertyi::read(skipws(p), getbsi(pe));
	p = read_variants(skipws(p), sb, pe->tags, pe);
	p = read_string(skipwscr(p), sb);
	pe->text = getstring(sb);
	return p;
}

static const char* read_answers(const char* p, short parent, stringbuilder& sb) {
	while(allowparse && isanswer(p)) {
		auto pe = bsdata<quest>::add(); pe->clear();
		pe->index = parent;
		p = stringbuilder::read(p, pe->next);
		p = read_variants(skipws(p), sb, pe->tags, pe);
		if(!checksym(p, ')'))
			break;
		p = read_string(skipws(p + 1), sb);
		pe->text = getstring(sb);
	}
	return p;
}

void quest::clear() {
	memset(this, 0, sizeof(*this));
}

const char* quest::getname(int id) {
	auto p = find(id);
	if(!p)
		return 0;
	return p->text;
}

bool quest::is(variant v) const {
	for(auto e : tags) {
		if(e.type == v.type && e.value == v.value)
			return true;
	}
	return false;
}

const quest* quest::find(short id) {
	for(auto& e : bsdata<quest>()) {
		if(e.index == id)
			return &e;
	}
	return 0;
}

const quest* quest::findprompt(short id) {
	for(auto& e : bsdata<quest>()) {
		if(e.index != id)
			continue;
		if(e.isanswer())
			continue;
		return &e;
	}
	return 0;
}

void quest::read(const char* url) {
	auto p = log::read(url);
	if(!p)
		return;
	char temp[4096]; stringbuilder sb(temp);
	allowparse = true;
	while(allowparse && *p) {
		if(!isevent(p)) {
			log::error(p, "Expected symbol `#` followed by event number");
			break;
		}
		short event_parent = -1; sb.clear();
		p = read_event(p + 1, event_parent, sb);
		p = read_answers(p, event_parent, sb);
	}
	log::close();
}

void quest::initialize() {
	propertyi::initialize();
	prop_header = propertyi::add("Header", propertyi::Text);
	prop_image = propertyi::add("Image", propertyi::Text);
}

void quest::manual(short page) {
	if(!answers::console)
		return;
	auto p = findprompt(page);
	while(p) {
		answers::console->clear();
		answers::console->add(p->text);
		answers an;
		auto index = p->index;
		auto pe = bsdata<quest>::end();
		for(auto pa = p + 1; pa < pe; pa++) {
			if(pa->index != index)
				continue;
			an.add(pa, pa->text);
		}
		auto pn = (quest*)an.choose(0, getnm("Back"), 1);
		if(!pn)
			break;
		manual(pn->next);
	}
}