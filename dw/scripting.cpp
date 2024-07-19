#include "main.h"
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
	if(bonus > 0)
		information(getnm("AddItem"), it.getname(), bonus);
	player->additem(it);
}

static void choose_player(int bonus) {
}

void quest::run(int index) {
	if(!answers::console)
		return;
	auto p = findprompt(index);
	answers an;
	while(p) {
		an.clear();
		auto push_last = an.last;
		player->act(p->text);
		script_run(p->tags);
		auto index = p->index;
		auto pe = bsdata<quest>::end();
		for(auto pa = p + 1; pa < pe; pa++) {
			if(pa->index != index)
				continue;
			if(!pa->isanswer())
				continue;
			if(!script_allow(pa->tags))
				continue;
			an.add(pa, pa->text);
		}
		an.last = push_last;
		auto pv = an.choose(0, 0, 1);
		if(!pv)
			break;
		if(bsdata<quest>::source.have(pv)) {
			p = (quest*)pv;
			p = findprompt(p->next);
		}
		answers::console->clear();
	}
	pause();
}

BSDATA(script) = {
	{"ChoosePlayer", choose_player},
};
BSDATAF(script)