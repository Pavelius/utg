#include "creature.h"
#include "console.h"
#include "pushvalue.h"
#include "quest.h"
#include "script.h"

static variant last;

static void fix_value(const char* format, const char* name, int value) {
	if(value > 0)
		information(format, name, value);
	else
		warning(format, name, -value);
}

static int getdiscountcost(int item_type, int bonus) {
	auto value = bsdata<itemi>::elements[item_type].coins * bonus;
	value -= player->get(Charisma);
	if(value < 0)
		value = 1;
	return value;
}

static int getcost(int item_type, int bonus) {
	return bsdata<itemi>::elements[item_type].coins * bonus;
}

template<> void fnscript<abilityi>(int index, int bonus) {
	fix_value("%1%+2i", getnm(bsdata<abilityi>::elements[index].id), bonus);
	player->abilities[index] += bonus;
}

template<> bool fntest<itemi>(int index, int bonus) {
	if(bonus < 0) {
		if(bsdata<itemi>::elements[index].tags.is(Coins))
			return getcost(index, -bonus);
		else
			return player->getuses(index) >= (-bonus);
	}
	return true;
}
template<> void fnscript<itemi>(int index, int bonus) {
	item it(index);
	if(bonus >= 0) {
		output(getnm("AddItem"), it.getname(), bonus ? bonus : 1);
		player->additem(it);
	}
}

static void choose_player(int bonus) {
}

static void apply_last_quest() {
	if(!last_quest)
		return;
	auto p = last_quest->getheader();
	if(p)
		answers::header = p;
	p = last_quest->getimage();
	if(p)
		answers::resid = p;
	player->act(last_quest->text);
	script_run(last_quest->tags);
}

static void* ask_player() {
	char temp[128]; stringbuilder sb(temp);
	sb.add(getnm("WhatPlayerWantToDo"), player->getname());
	return an.choose(temp, 0, 1);
}

void quest_run(int index) {
	if(!answers::console)
		return;
	last_quest = quest_find_prompt(index);
	while(last_quest) {
		an.clear();
		apply_last_quest();
		auto index = last_quest->index;
		auto pe = bsdata<quest>::end();
		for(auto pa = last_quest + 1; pa < pe; pa++) {
			if(pa->index != index)
				continue;
			if(!pa->isanswer())
				continue;
			if(!script_allow(pa->tags))
				continue;
			an.add(pa, pa->text);
		}
		auto last_result = ask_player();
		clear_console();
		an.console->clear();
		if(!last_result)
			break;
		if(bsdata<quest>::source.have(last_result)) {
			last_quest = (quest*)last_result;
			script_run(last_quest->tags);
			last_quest = quest_find_prompt(last_quest->next);
		}
	}
	pause();
}

BSDATA(script) = {
	{"ChoosePlayer", choose_player},
};
BSDATAF(script)