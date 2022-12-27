#include "answers.h"
#include "actable.h"
#include "crt.h"
#include "creature.h"
#include "groupname.h"
#include "pushvalue.h"
#include "rang.h"
#include "roll.h"
#include "wise.h"

hero			*player, *opponent;
heroa			party;
static answers	an;

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
	if(!maximum_count)
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

static void add_new_wises(int bonus) {
	for(auto i = 0; i < bonus; i++) {
		auto p = choose_wise(getnm("ChooseKnownWise"), i, bonus);
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

static void add_player_rang() {
	auto p = choose_rang();
	player->setrang(p);
	player->setskills(p->skills);
}

static const skilli* choose_skill(const char* header, const variants& source) {
	an.clear();
	for(auto v : source) {
		skilli* p = v;
		if(!p)
			continue;
		if(ismarked(p))
			continue;
		an.add(p, p->getname());
	}
	return (skilli*)an.choose(header);
}

static void add_skill(const char* header, const variants& source) {
	marked.clear();
	auto p = choose_skill(header, source);
	raise_skill(p);
}

static void add_player_born() {
	auto p = choose_born();
	player->setborn(p);
	add_skill(getnm("YouNativeSkill"), player->getborn()->skills);
}

void hero::create() {
	pushvalue push_player(player, this);
	clear();
	setname(groupname::randomid("MouseguardMale"));
	add_player_rang();
	add_player_born();
	add_new_wises(bsdata<rangi>::elements[rang].wises);
}