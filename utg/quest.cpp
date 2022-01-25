#include "answers.h"
#include "log.h"
#include "quest.h"

BSDATAC(quest, 2048)

using namespace log::parse;

stringbuilder* quest::console;

static const char* skipnum(const char* p) {
	while(isnum(*p))
		p++;
	return p;
}

static bool isanswer(const char* p) {
	return isnum(*p);
}

static bool isevent(const char* p) {
	return p[0] == '#' && isnum(p[1]);
}

static const char* read_string(const char* p, stringbuilder& result) {
	result.clear();
	if(p[0] == '#')
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

static const char* read_identifier(const char* p, stringbuilder& result) {
	result.clear();
	return result.psidf(p);
}

static const char* getstring(stringbuilder& sb) {
	auto p = sb.begin();
	if(!p[0])
		return 0;
	return szdup(p);
}

static const char* read_params(const char* p, stringbuilder& result, bool& allow_continue) {
	if(p[0] != '(') {
		log::error(p, "Expected symbol `(`");
		allow_continue = false;
		return p;
	}
	p = read_identifier(skipws(p + 1), result);
	if(p[0] != ')') {
		log::error(p, "Expected symbol `(`");
		allow_continue = false;
		return p;
	}
	p = skipws(p + 1);
	return p;
}

static const char* read_bonus(const char* p, int& bonus) {
	if(*p == '-')
		p = stringbuilder::read(p, bonus);
	else if(*p == '+')
		p = stringbuilder::read(p + 1, bonus);
	else
		bonus = 0;
	return p;
}

static const char* read_variants(const char* p, stringbuilder& result, variants& source, bool& allow_continue, quest* pe) {
	while(allow_continue && ischa(*p)) {
		p = read_identifier(p, result);
		p = skipws(p);
		auto pn = result.begin();
		if(equal(pn, "image")) {
			p = read_params(p, result, allow_continue);
			pe->image = getstring(result);
		} else {
			int bonus; p = read_bonus(p, bonus);
			p = skipws(p);
			variant v = (const char*)result.begin();
			if(!v)
				log::error(p, "Can't find variant `%1`", result.begin());
			else
				v.counter = bonus;
			add(source, v);
		}
	}
	return p;
}

static const char* skipcr(const char* p, bool& allow_continue) {
	if(p[0] == 10 || p[0] == 13)
		p = skipwscr(p);
	else {
		log::error(p, "Expected line feed");
		allow_continue = false;
	}
	return p;
}

static const char* read_event(const char* p, short& parent, stringbuilder& sb, bool& allow_continue) {
	if(!allow_continue)
		return p;
	p = stringbuilder::read(skipws(p), parent);
	auto pe = bsdata<quest>::add(); pe->clear();
	pe->index = parent;
	pe->next = -1;
	p = read_variants(skipws(p), sb, pe->tags, allow_continue, pe);
	p = read_string(skipwscr(p), sb);
	pe->text = getstring(sb);
	return p;
}

static const char* read_answers(const char* p, short parent, stringbuilder& sb, bool& allow_continue) {
	while(allow_continue && isanswer(p)) {
		auto pe = bsdata<quest>::add(); pe->clear();
		pe->index = parent;
		p = stringbuilder::read(p, pe->next);
		p = read_variants(skipws(p), sb, pe->tags, allow_continue, pe);
		if(p[0] != ')') {
			log::error(p, "Expected symbol `)` after a number");
			allow_continue = false;
			break;
		}
		p = read_string(skipws(p + 1), sb);
		pe->text = getstring(sb);
	}
	return p;
}

void quest::clear() {
	memset(this, 0, sizeof(*this));
}

void quest::read(const char* url) {
	auto p_alloc = (const char*)loadt(url);
	if(!p_alloc)
		return;
	log::seturl(url);
	auto p = p_alloc;
	log::setfile(p);
	variants resolves;
	char temp[4096]; stringbuilder sb(temp);
	auto allow_continue = true;
	while(allow_continue && p[0]) {
		if(!isevent(p)) {
			log::error(p, "Expected symbol `#` and followed event identifier");
			break;
		}
		short event_parent = -1; sb.clear();
		p = read_event(p + 1, event_parent, sb, allow_continue);
		p = read_answers(p, event_parent, sb, allow_continue);
	}
	delete p_alloc;
}

static bool isallow(const variants& source) {
	if(!variant::sfapply)
		return true;
	for(auto v : source) {
		if(!variant::sfapply(v, false))
			return false;
	}
	return true;
}

static void apply(const variants& source) {
	if(!variant::sfapply)
		return;
	for(auto v : source)
		variant::sfapply(v, true);
}

const quest* quest::findprompt(short id) {
	for(auto& e : bsdata<quest>()) {
		if(e.index != id)
			continue;
		if(e.isanswer())
			continue;
		if(!isallow(e.tags))
			continue;
		return &e;
	}
	return 0;
}

const quest* quest::choose(int id, const char* resid, const char* header) const {
	const char* promt_text = text;
	if(console) {
		console->clear();
		console->add(text);
		promt_text = console->begin();
	}
	auto need_add_continue = true;
	apply(tags);
	answers an;
	for(auto& e : bsdata<quest>()) {
		if(e.index != id)
			continue;
		if(!e.isanswer())
			continue;
		if(!isallow(e.tags))
			continue;
		an.add(&e, e.text);
	}
	return (quest*)an.choose(0, 0, true, resid, -1, header, promt_text);
}

void quest::run(int id, const char* resid, const char* header) {
	while(true) {
		auto p = findprompt(id);
		if(!p)
			return;
		if(p->image)
			resid = p->image;
		p = p->choose(p->index, resid, header);
		if(!p)
			break;
		apply(p->tags);
		id = p->next;
	}
}