#include "main.h"
#include "stringact.h"

class player::string : public stringact {
	const player& source;
	static void addvname(stringbuilder& sb, variant v, int padeg = 0) {
		if(!v)
			return;
		auto pv = (groupvaluei*)v;
		if(pv) {
			switch(padeg) {
			case 1: sb.addby(pv->name); break;
			default: sb.add(pv->name); break;
			}
		}
	}
	static void addvalue(stringbuilder& sb, variant v) {
		groupvaluei* pv = v;
		if(pv)
			sb.adds(pv->name);
		else
			sb.adds(v.getname());
	}
	static void show_values(stringbuilder& sb, const player& source) {
		auto index = 0;
		for(auto v : source.values) {
			if(v) {
				sb.addn("* ");
				sb.add("%1", game.getclass().types[index].getname());
				addvalue(sb, v);
			}
			index++;
		}
		sb.addsep('\n');
	}
	static void show_crew(stringbuilder& sb, const char* separator, const char* padding, bool hilite) {
		for(auto& e : game.getfriends()) {
			sb.addn(padding);
			if(hilite)
				sb.add("[");
			e.getname(sb);
			if(hilite)
				sb.add("]");
			sb.add(separator);
		}
	}
	void addidentifier(const char* identifier) override {
		if(equal(identifier, "герой"))
			source.getactive().getname(*this);
		else if(equal(identifier, "Value1"))
			addvname(*this, source.values[0]);
		else if(equal(identifier, "ByValue1"))
			addvname(*this, source.values[0], 1);
		else if(equal(identifier, "Value2"))
			addvname(*this, source.values[1]);
		else if(equal(identifier, "ByValue2"))
			addvname(*this, source.values[1], 1);
		else if(equal(identifier, "Value3"))
			addvname(*this, source.values[2]);
		else if(equal(identifier, "ByValue3"))
			addvname(*this, source.values[2], 1);
		else if(equal(identifier, "Value4"))
			addvname(*this, source.values[3]);
		else if(equal(identifier, "ByValue4"))
			addvname(*this, source.values[3], 1);
		else if(equal(identifier, "Value5"))
			addvname(*this, source.values[4]);
		else if(equal(identifier, "ByValue5"))
			addvname(*this, source.values[4], 1);
		else if(equal(identifier, "Class"))
			add(getnm(source.getclass().id));
		else if(equal(identifier, "ShowCrew"))
			show_crew(*this, "\n", "* ", true);
		else if(equal(identifier, "ShowValues"))
			show_values(*this, source);
		else
			stringact::addidentifier(identifier);
	}
public:
	string(const player& source, const stringbuilder& sb) :
		source(source),
		stringact(sb, source.getactive().getname(), source.getgender()) {
	}
};

static void* ask(answers& an, const char* id) {
	if(!id)
		return an.choose(0);
	char temp[260]; stringbuilder sb(temp);
	game.actn(sb, getnm(id), 0, false);
	return an.choose(temp);
}

static int choose(array& source, const char* id) {
	answers an;
	auto pe = source.end();
	auto sz = source.getsize();
	for(auto p = source.begin(); p < source.end(); p += sz)
		an.add(p, getnm(*(const char**)p));
	return source.indexof(ask(an, id));
}

void player::chooseclass() {
	classid = choose(bsdata<classi>::source, 0);
}

void player::actn(stringbuilder& sbs, const char* format, const char* format_param, bool add_sep) const {
	string sb(*this, sbs);
	if(add_sep)
		sb.addsep('\n');
	sb.addv(format, format_param);
	sbs = sb;
}

void player::generate() {
	randomname();
	for(auto& e : friends)
		e.randomname();
	game.script(AnswerStartGame);
	chooseclass();
	choosehistory();
}

static variant choose_answer(const player* source, variant group) {
	return groupi::choose(group);
}

void player::choosehistory() {
	auto index = 0;
	for(auto v : getclass().types) {
		utg::sb.clear();
		game.script(AnswerStartGame + 1);
		values[index++] = choose_answer(this, v);
	}
	utg::sb.clear();
	game.script(AnswerStartGame + 1);
	draw::pause(getnm("WhatAboutHistory"));
	background();
}

void player::shuffleparcipant() {
	for(unsigned i = 0; i < maxcount; i++)
		order[i] = i;
	zshuffle(order, maxcount);
}

const messagei* find_message(variant type, int value) {
	for(auto& e : bsdata<messagei>()) {
		if(e.type == type && e.value == value)
			return &e;
	}
	return 0;
}

void player::background() const {
	if(!answers::interactive)
		return;
	auto pn = find_message(variant(Class, classid), 4);
	if(!pn)
		return;
	utg::sb.clear();
	act(pn->text);
	draw::pause();
}