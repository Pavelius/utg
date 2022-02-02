#include "main.h"
#include "stringact.h"

static void show_crew(stringbuilder& sb, const char* separator, const char* padding, bool hilite) {
	for(auto& e : bsdata<pirate>()) {
		sb.addn(padding);
		if(hilite)
			sb.add("[");
		e.getname(sb);
		if(hilite)
			sb.add("]");
		sb.adds("%-Pirate-%-1", getnm(e.getclass().id));
		sb.add(separator);
	}
}

static void addvalue(stringbuilder& sb, variant v) {
	groupvaluei* pv = v;
	if(pv)
		sb.adds(pv->name);
	else
		sb.adds(v.getname());
}

class historyable::string : public stringact {
	const historyable& source;
	static void show_values(stringbuilder& sb, const historyable& source) {
		auto index = 0;
		auto& ei = source.getclass();
		for(auto v : source.values) {
			if(v) {
				sb.addn("* ");
				sb.add("%1 - ", ei.types[index].getname());
				addvalue(sb, v);
			}
			index++;
		}
		sb.addsep('\n');
	}
	void addvname(variant v, int padeg = 0) {
		if(!v)
			return;
		auto pv = (groupvaluei*)v;
		if(pv) {
			switch(padeg) {
			case 1: addby(pv->name); break;
			default: add(pv->name); break;
			}
		}
	}
	void addidentifier(const char* identifier) override {
		if(equal(identifier, "Value1"))
			addvname(source.values[0]);
		else if(equal(identifier, "ByValue1"))
			addvname(source.values[0], 1);
		else if(equal(identifier, "Value2"))
			addvname(source.values[1]);
		else if(equal(identifier, "ByValue2"))
			addvname(source.values[1], 1);
		else if(equal(identifier, "Value3"))
			addvname(source.values[2]);
		else if(equal(identifier, "ByValue3"))
			addvname(source.values[2], 1);
		else if(equal(identifier, "Value4"))
			addvname(source.values[3]);
		else if(equal(identifier, "ByValue4"))
			addvname(source.values[3], 1);
		else if(equal(identifier, "Value5"))
			addvname(source.values[4]);
		else if(equal(identifier, "ByValue5"))
			addvname(source.values[4], 1);
		else if(equal(identifier, "герой"))
			source.getname(*this);
		else if(equal(identifier, "ShowCrew"))
			show_crew(*this, "\n", "* ", true);
		else if(equal(identifier, "ShowValues"))
			show_values(*this, source);
		else
			stringact::addidentifier(identifier);
	}
public:
	string(const historyable& source, const stringbuilder& sb) :
		source(source),
		stringact(sb, source.getname(), source.getgender()) {
	}
};

void historyable::clear() {
	memset(this, 0, sizeof(*this));
	classid = 0xFFFF;
	npcname::clear();
}

void historyable::actn(stringbuilder& sbs, const char* format, const char* format_param) const {
	string sb(*this, sbs);
	sb.addsep('\n');
	sb.addv(format, format_param);
	sbs = sb;
}

void historyable::chooseclass() {
	answers an;
	for(auto& e : bsdata<classi>()) {
		if(game.match(&e))
			continue;
		an.add((void*)(&e - bsdata<classi>::elements), getnm(e.id));
	}
	char temp[260]; temp[0] = 0;
	stringbuilder sb(temp);
	act(sb, getnm("ChooseClass"));
	classid = (int)an.choose(temp);
}

static variant choose_answer(const historyable* source, variant group) {
	return groupi::choose(group);
}

const messagei* find_message(variant type, int value) {
	for(auto& e : bsdata<messagei>()) {
		if(e.type == type && e.value == value)
			return &e;
	}
	return 0;
}

void historyable::background() const {
	if(!answers::interactive)
		return;
	auto pn = find_message(variant(Class, classid), 4);
	if(!pn)
		return;
	utg::sb.clear();
	act(utg::sb, pn->text);
	draw::pause();
}

void historyable::choosehistory() {
	auto index = 0;
	for(auto v : getclass().types) {
		utg::sb.clear();
		act(utg::sb, getnm("PromtHistory"));
		values[index++] = choose_answer(this, v);
	}
	utg::sb.clear();
	act(utg::sb, getnm("PromtHistory"));
	draw::pause(getnm("WhatAboutHistory"));
	background();
}