#include "answers.h"
#include "log.h"
#include "quest.h"

BSDATAC(quest, 4096)

// 1) One/Two on none events occurs in province of each player. Each player have same cards deck. About 30-40 cards.
// 2) One event occurs in random province occuped by each player. This is separate deck and it is suppose to be bad.
// 3) You can mix to some part of player deck (or neutral deck) additional cards.

static const char* skipnum(const char* p) {
	while(isnum(*p))
		p++;
	return p;
}

static bool isanswer(const char* p) {
	auto p1 = skipnum(p);
	return p1 != p && p1[0] == ')';
}

static bool isevent(const char* p) {
	return p[0] == '#' && isnum(p[1]);
}

static const char* read_string(const char* p, stringbuilder& result) {
	result.clear();
	if(p[0] == '#')
		return p;
	while(*p) {
		char sym;
		if(*p == '\n' || *p == '\r') {
			p = skipspcr(p);
			if(p[0] == '#' || isanswer(p))
				break;
			sym = '\n';
		} else
			sym = *p++;
		switch(sym) {
		case 17: sym = '-'; break;
		}
		result.add(sym);
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
	p = read_identifier(skipsp(p + 1), result);
	if(p[0] != ')') {
		log::error(p, "Expected symbol `(`");
		allow_continue = false;
		return p;
	}
	p = skipsp(p + 1);
	return p;
}

static const char* read_variants(const char* p, stringbuilder& result, variants& source, bool& allow_continue, quest* pe) {
	while(allow_continue && ischa(*p)) {
		p = read_identifier(p, result);
		p = skipsp(p);
		auto pn = result.begin();
		if(equal(pn, "image")) {
			p = read_params(p, result, allow_continue);
			pe->image = getstring(result);
		} else {
			variant v = (const char*)result.begin();
			if(!v)
				log::error(p, "Can't find variant `%1`", result.begin());
			add(source, v);
		}
	}
	return p;
}

static const char* skipcr(const char* p, bool& allow_continue) {
	if(p[0] == 10 || p[0] == 13)
		p = skipspcr(p);
	else {
		log::error(p, "Expected line feed");
		allow_continue = false;
	}
	return p;
}

static const char* read_event(const char* p, short& parent, stringbuilder& sb, bool& allow_continue) {
	if(!allow_continue)
		return p;
	p = stringbuilder::read(skipsp(p), parent);
	auto pe = bsdata<quest>::add(); pe->clear();
	pe->index = parent;
	pe->next = -1;
	p = read_variants(skipsp(p), sb, pe->tags, allow_continue, pe);
	p = read_string(skipspcr(p), sb);
	pe->text = getstring(sb);
	return p;
}

static const char* read_answers(const char* p, short parent, stringbuilder& sb, bool& allow_continue) {
	while(allow_continue && isanswer(p)) {
		auto pe = bsdata<quest>::add(); pe->clear();
		pe->index = parent;
		p = stringbuilder::read(p, pe->next);
		p = read_variants(skipsp(p), sb, pe->tags, allow_continue, pe);
		if(p[0]!=')') {
			log::error(p, "Expected symbol `)` after a number");
			break;
		}
		p = read_string(skipsp(p + 1), sb);
		pe->text = getstring(sb);
	}
	return p;
}

quest::fnallow quest::allow;
quest::fnallow quest::apply;

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
	if(!quest::allow)
		return true;
	for(auto v : source) {
		if(!quest::allow(v))
			return false;
	}
	return true;
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

const quest* quest::choose(int id, const char* title, const char* resid, const char* header) const {
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
	return (quest*)an.choose(title, 0, true, resid, -1, header, text);
}

void quest::run(int id, const char* title, const char* resid, const char* header) {
	while(true) {
		auto p = findprompt(id);
		if(!p)
			return;
		for(auto v : p->tags)
			apply(v);
		if(p->image)
			resid = p->image;
		p = p->choose(p->index, title, resid, header);
		for(auto v : p->tags)
			apply(v);
		id = p->next;
	}
}