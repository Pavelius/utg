#include "answers.h"
#include "actable.h"
#include "crt.h"
#include "groupname.h"
#include "list.h"
#include "hero.h"
#include "pushvalue.h"
#include "questlist.h"
#include "rang.h"
#include "roll.h"
#include "wise.h"

hero			*player, *opponent;
heroa			party;
static answers	an;

void questrun(const char* id);

static int get_rang_count(const rangi* rang) {
	auto result = 0;
	for(auto p : party) {
		if(p->getrang() == rang)
			result++;
	}
	return result;
}

static const rangi* choose_rang() {
	an.clear();
	for(auto& e : bsdata<rangi>()) {
		if(e.party_count && get_rang_count(&e) >= e.party_count)
			continue;
		an.add(&e, e.getname());
	}
	return (rangi*)an.choose(getnm("ChooseRang"));
}

static const char* choose_options(const char* header, int count, int maximum_count) {
	static char temp[260];
	if(maximum_count <= 1)
		return header;
	stringbuilder sb(temp);
	sb.add(header); sb.adds("(%Left [%1i]):", maximum_count - count);
	return temp;
}

static const enviromenti* choose_born() {
	an.clear();
	for(auto& e : bsdata<enviromenti>()) {
		if(e.type == Wilderness)
			continue;
		an.add(&e, e.getname());
	}
	return (enviromenti*)an.choose(getnm("WhereWereYouBorn"));
}

static const wisei* choose_wise(const char* header, int count, int maximum_count) {
	pushvalue columns(answers::column_count, 2);
	an.clear();
	for(auto& e : bsdata<wisei>()) {
		if(player->iswise(&e))
			continue;
		an.add(&e, e.getname());
	}
	return (wisei*)an.choose(choose_options(header, count, maximum_count));
}

static const traiti* choose_trait(const char* header, const variants& source) {
	an.clear();
	for(auto v : source) {
		auto p = (traiti*)v;
		if(!p)
			continue;
		an.add(p, p->getname());
	}
	return (traiti*)an.choose(header);
}

static void add_new_wises(int bonus) {
	for(auto i = 0; i < bonus; i++) {
		auto p = choose_wise(getnm("YouNewWises"), i, bonus);
		player->setwise(p);
	}
}

static void raise_skill(const skilli* p) {
	auto i = (skill_s)(p - bsdata<skilli>::elements);
	if(player->getskill(i))
		player->setskill(i, 2);
	else if(player->getskill(i) < 6)
		player->addskill(i, 1);
}

static void raise_trait(const traiti* p) {
	auto i = (trait_s)getbsi(p);
	if(player->gettrait(i) < 3)
		player->settrait(i, player->gettrait(i) + 1);
}

static void add_player_rang() {
	auto p = choose_rang();
	player->setrang(p);
	player->setskills(p->skills);
}

static const skilli* choose_skill(const char* header, const variants& source, int columns, int count, int maximum_count) {
	an.clear();
	for(auto v : source) {
		skilli* p = v;
		if(!p)
			continue;
		if(ismarked(p))
			continue;
		an.add(p, p->getname());
	}
	pushvalue push_columns(answers::column_count, columns);
	return (skilli*)an.choose(choose_options(header, count, maximum_count));
}

static void add_skill(const char* header, const variants& source, int columns) {
	marked.clear();
	auto p = choose_skill(header, source, columns, 0, 0);
	raise_skill(p);
}

static void add_skill(const char* header, const variants& source, int columns, int maximum_count, skill_s* result) {
	marked.clear();
	for(auto i = 0; i < maximum_count; i++) {
		auto p = choose_skill(header, source, columns, i, maximum_count);
		marked.add(p);
		raise_skill(p);
		if(result)
			*result = (skill_s)getbsi(p);
	}
}

static void add_trait(const char* header, const variants& source) {
	marked.clear();
	auto p = choose_trait(header, source);
	raise_trait(p);
}

static void add_player_born() {
	auto p = choose_born();
	player->setborn(p);
	add_skill(getnm("YouNativeSkill"), player->getborn()->skills, 1);
	add_trait(getnm("YouNativeTrait"), player->getborn()->traits);
}

static void add_skill_list(const char* id, int columns, int count, skill_s* result = 0) {
	auto list = bsdata<listi>::find(id);
	if(!list)
		return;
	add_skill(getnm(id), list->elements, columns, count, result);
}

static void add_quest(const char* id) {
	auto p = bsdata<questlist>::find(id);
	if(!p)
		return;
	pushvalue push_quest(lastquest, p);
}

void hero::clear() {
	memset(this, 0, sizeof(*this));
	setname(0xFFFF);
}

void hero::create() {
	pushvalue push_player(player, this);
	clear();
	setname(groupname::randomid("MouseguardMale"));
	add_player_rang();
	add_player_born();
	add_skill_list("YouTalentedSkills", 2, getrang()->talented);
	add_skill_list("YouParentSkills", 2, 1);
	add_skill_list("YouSocialSkills", 1, getrang()->convice);
	add_skill_list("YouMasterCrafterSkills", 1, 1);
	add_skill_list("YouMentorSkills", 2, getrang()->mentors);
	add_skill_list("YouSpecializationSkills", 2, getrang()->specialization, &specialization);
	questrun("NatureQuest");
	add_new_wises(getrang()->wises);
}