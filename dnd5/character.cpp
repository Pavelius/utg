#include "ability.h"
#include "answers.h"
#include "advance.h"
#include "class.h"
#include "character.h"
#include "list.h"
#include "modifier.h"
#include "pushvalue.h"
#include "race.h"
#include "script.h"
#include "skill.h"

typedef adat<char, 6> abilitya;

character *player;
static char ability_places[6];

static void apply_abilities(abilitya& source, const listi* abilitites) {
	if(!abilitites)
		return;
	char temp[512]; stringbuilder sb(temp);
	sb.add("%1: ", getnm("NeedRellocateAbility"));
	auto pn = sb.get();
	for(auto v : source) {
		if(pn[0])
			sb.add(", ");
		sb.adds("%1i", v);
	}
	sb.add(".");
	sb.addn(getnm("WhereYouWantRelocate"), source.data[0]);
	answers an;
	for(auto v : abilitites->elements) {
		auto pv = (abilityi*)v.getpointer();
		auto n = pv - bsdata<abilityi>::elements;
		if(ability_places[n])
			continue;
		an.add(pv, v.getname());
	}
	pushvalue push_column(answers::column_count, 1);
	auto pv = (abilityi*)an.choose(temp, 0, 2);
	auto n = pv - bsdata<abilityi>::elements;
	player->abilitites[n] += source.data[0];
	ability_places[n] = source.data[0];
	source.remove(0);
}

static void apply_abilities(character* player_this) {
	pushvalue push_resid(player, player_this);
	variant list = "AbilitiesList";
	if(!list)
		return;
	memset(ability_places, 0, sizeof(ability_places));
	abilitya abilities;
	abilities.add(15);
	abilities.add(14);
	abilities.add(13);
	abilities.add(12);
	abilities.add(10);
	abilities.add(8);
	while(abilities.getcount() > 0)
		apply_abilities(abilities, (listi*)list.getpointer());
}

void character::advance(variant object, int level) {
	pushvalue push_player(player, this);
	for(auto& e : bsdata<advancei>()) {
		if(e.object == object && e.level == level)
			e.apply();
	}
}

void character::clear() {
	memset(this, 0, sizeof(*this));
}

void character::generate() {
	pushvalue push_resid(answers::resid, "generate");
	pushvalue push_header(answers::header, getnm("CharacterGenerate"));
	pushvalue push_column(answers::column_count, -1);
	clear();
	advance("CharacterGenerate");
	apply_abilities(this);
	racei* race = getkind();
	if(race) {
		if(race->parent)
			advance(race->parent, 0);
		if(race)
			advance(race, 0);
	}
	for(auto i = 0; i < sizeof(classes) / sizeof(classes[0]); i++) {
		if(!classes[i])
			continue;
		advance(bsdata<classi>::elements + i, classes[i]);
	}
}

int character::getskill(int v) const {
	auto& ei = bsdata<skilli>::elements[v];
	auto result = getbonus(ei.ability);
	if(skills.is(v))
		result += abilitites[Proficiency];
	return result;
}