#include "creature.h"
#include "console.h"
#include "pushvalue.h"
#include "quest.h"
#include "script.h"

static variant last;
static void* last_choose;

static int get_cost_discounted(int item_type, int bonus) {
	auto value = bsdata<itemi>::elements[item_type].coins * bonus;
	value -= player->get(Charisma);
	if(value < 0)
		value = 1;
	return value;
}

static void fix(const char* id, ...) {
	auto p = getdescription(id);
	if(!p)
		return;
	add_console(' ', p, xva_start(id));
}

static int getcost(int item_type, int bonus) {
	return bsdata<itemi>::elements[item_type].coins * bonus;
}

template<> void fnscript<abilityi>(int index, int bonus) {
	output("%1%+2i", getnm(bsdata<abilityi>::elements[index].id), bonus);
	player->abilities[index] += bonus;
}

template<> bool fntest<itemi>(int index, int bonus) {
	if(bonus < 0) {
		if(bsdata<itemi>::elements[index].tags.is(Coins))
			return player->coins >= getcost(index, -bonus);
		else
			return player->getuses(index) >= (-bonus);
	}
	return true;
}
template<> void fnscript<itemi>(int index, int bonus) {
	item it(index);
	if(bonus >= 0) {
		if(bonus > 1)
			fix("AddItems", it.getname(), bonus);
		else
			fix("AddItem", it.getname());
		player->additem(it);
	} else {
		auto& ei = bsdata<itemi>::elements[index];
		bonus = -bonus;
		if(ei.tags.is(Coins))
			fix("RemoveCoins", bonus);
		else if(bonus>1)
			fix("RemoveItems", ei.getname(), bonus);
		else
			fix("RemoveItem", ei.getname());
		player->removeitem(index, bonus);
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
	act(last_quest->text);
	script_run(last_quest->tags);
}

static void ask_player() {
	char temp[128]; stringbuilder sb(temp);
	sb.add(getnm("WhatPlayerWantToDo"), player->getname());
	last_choose = an.choose(temp, 0, 1);
}

static void quest_add_answers() {
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
}

static bool choose_need_next() {
	if(!last_choose)
		return true;
	if(bsdata<creature>::source.have(last_choose)) {
		player = (creature*)last_choose;
		return false;
	}
	return true;
}

static void choose_apply() {
	if(bsdata<quest>::source.have(last_choose)) {
		last_quest = (quest*)last_choose;
		script_run(last_quest->tags);
		last_quest = quest_find_prompt(last_quest->next);
	}
}

void quest_run(int index) {
	if(!answers::console)
		return;
	last_quest = quest_find_prompt(index);
	while(last_quest) {
		apply_last_quest();
		while(true) {
			an.clear();
			quest_add_answers();
			ask_player();
			if(choose_need_next())
				break;
		}
		clear_console();
		an.console->clear();
		if(!last_choose)
			break;
		choose_apply();
	}
	pause();
}

BSDATA(script) = {
	{"ChoosePlayer", choose_player},
};
BSDATAF(script)