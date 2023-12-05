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

static const quest* choose_answers(const char* header, bool skip_marked = true) {
	an.clear();
	auto index = last_quest->index;
	auto pb = last_quest + 1;
	for(auto pe = last_questlist->elements.end(); pb < pe; pb++) {
		if(pb->index != index)
			break;
		if(skip_marked && marked.find(pb) != -1)
			continue;
		an.add(pb, pb->text);
	}
	return (quest*)an.choose(header);
}

static void choose_list(int bonus) {
	marked.clear();
	pushvalue push_last(last_quest, last_questlist->find(1000, last_quest + 1));
	if(!last_quest)
		return;
	auto header = last_quest->text;
	for(auto i = 0; i < bonus; i++) {
		auto p = choose_answers(header);
		marked.add(p);
	}
}

static void questrun(int index) {
	pushvalue push_last(last_quest);
	last_quest = last_questlist->find(index, 0);
	while(last_quest) {
		player->act(last_quest->text);
		if(last_quest->tags)
			script_run(last_quest->tags);
		auto p = choose_answers(0);
		if(p->tags)
			script_run(p->tags);
		last_quest = last_questlist->find(p->next);
		answers::console->clear();
	}
}

void questrun(const char* id) {
	auto p = bsdata<questlist>::find(id);
	if(!p)
		return;
	pushvalue push_list(last_questlist, p);
	questrun(1);
}

BSDATA(script) = {
	{"Choose", choose_list},
};
BSDATAF(script)