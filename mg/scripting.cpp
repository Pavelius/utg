#include "actable.h"
#include "hero.h"
#include "questlist.h"
#include "pushvalue.h"
#include "skill.h"
#include "script.h"

template<> void fnscript<skilli>(int index, int bonus) {
	player->addskill((skill_s)index, bonus);
}

template<> void fnscript<traiti>(int index, int bonus) {
	auto i = (trait_s)index;
	if(player->gettrait(i) < 3)
		player->settrait(i, player->gettrait(i) + 1);
}

static const quest* choose_answers(const char* header) {
	an.clear();
	auto index = last_quest->index;
	auto pb = last_quest + 1;
	for(auto pe = last_questlist->elements.end(); pb < pe; pb++) {
		if(pb->index != index)
			break;
		an.add(pb, pb->text);
	}
	return (quest*)an.choose(header);
}

static void choose_list(int bonus) {
	marked.clear();
	pushvalue push_last(quest::last, last_questlist->find(1000, quest::last + 1));
	if(!quest::last)
		return;
	auto header = quest::last->text;
	for(auto i = 0; i < bonus; i++) {
		auto p = choose_answers(header);
		marked.add(p);
	}
}

static void questrun(int index) {
	pushvalue push_last(quest::last);
	quest::last = lastquest->find(index, 0);
	while(quest::last) {
		player->act(quest::last->text);
		if(quest::last->tags)
			script::run(quest::last->tags);
		auto p = choose_answers(0);
		if(p->tags)
			script::run(p->tags);
		quest::last = lastquest->find(p->next);
		answers::console->clear();
	}
}

void questrun(const char* id) {
	auto p = bsdata<questlist>::find(id);
	if(!p)
		return;
	pushvalue push_list(lastquest, p);
	questrun(1);
}

BSDATA(script) = {
	{"Choose", choose_list},
};
BSDATAF(script)