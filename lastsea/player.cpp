#include "main.h"
#include "rand.h"

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
	auto sz = source.element_size;
	for(auto p = source.begin(); p < source.end(); p += sz)
		an.add(p, getnm(*(const char**)p));
	return source.indexof(ask(an, id));
}

void player::chooseclass() {
	classid = choose(bsdata<classi>::source, "ChooseClass");
}

static void copy(npcname& e1, npcname& e2) {
	e1 = e2;
}

const npcname* player::getplayer(int i) const {
	switch(i) {
	case 0: return this;
	case 1: return friends + 0;
	case 2: return friends + 1;
	case 3: return friends + 2;
	default: return 0;
	}
}

void player::choosename() {
	npcname names[8];
	answers an;
	char temp[260]; stringbuilder sb(temp);
	for(auto& e : names) {
		e.randomname();
		sb.clear(); e.getname(sb);
		an.add(&e, temp);
	}
	auto p = (npcname*)an.choose(getnm("ChooseName"));
	copy(*this, *p);
}

void player::actn(stringbuilder& sbs, const char* format, const char* format_param, bool add_sep) const {
	if(add_sep)
		sbs.addsep('\n');
	sbs.addv(format, format_param);
}

void player::generate() {
	randomname();
	for(auto& e : friends)
		e.randomname();
	game.script(AnswerStartGame);
	choosename();
	chooseclass();
	choosehistory();
}

static variant choose_answer(const player* source, variant group) {
	if(group.type == Group && group.value <= 1)
		return group;
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
	pause(getnm("WhatAboutHistory"));
	background();
}

void player::shuffleparcipant() {
	for(unsigned i = 0; i < maxcount; i++)
		order[i] = i;
	zshuffle(order, maxcount);
}