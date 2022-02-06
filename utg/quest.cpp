#include "answers.h"
#include "logparse.h"
#include "quest.h"

BSDATAC(quest, 2048)

propertyi::indext quest::prop_image;
propertyi::indext quest::prop_header;

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

static bool isallow(const variants& source, bool run) {
	if(!variant::sfapply)
		return true;
	for(auto v : source) {
		if(!variant::sfapply(v, run))
			return false;
	}
	return true;
}

void quest::apply(const variants& source) {
	isallow(source, true);
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
		if(!isallow(e.tags, false))
			continue;
		return &e;
	}
	return 0;
}

const quest* quest::choose(int id) const {
	if(answers::console) {
		answers::console->clear();
		answers::console->add(text);
	}
	apply(tags);
	answers an;
	for(auto& e : bsdata<quest>()) {
		if(e.index != id)
			continue;
		if(!e.isanswer())
			continue;
		if(!isallow(e.tags, false))
			continue;
		an.add(&e, e.text);
	}
	return (quest*)an.choose(0, 0);
}

void quest::run(int id) {
	while(true) {
		auto p = findprompt(id);
		if(!p)
			return;
		auto pv = p->getheader();
		if(pv)
			answers::header = pv;
		pv = p->getimage();
		if(pv)
			answers::resid = pv;
		p = p->choose(p->index);
		if(!p)
			break;
		apply(p->tags);
		id = p->next;
	}
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