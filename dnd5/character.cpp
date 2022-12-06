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

character *player;
static char ability_places[6];
void apply_modifier(modifier_s v, int bonus);

static void add_elements(answers& an, const variants& elements) {
	for(auto v : elements) {
		if(v.iskind<listi>())
			add_elements(an, bsdata<listi>::elements[v.value].elements);
		else if(v.iskind<modifieri>())
			apply_modifier((modifier_s)v.value, v.counter);
		else {
			if(script::isallow(v))
				an.add(v.getpointer(), v.getname());
		}
	}
}

static variant choose_advance(const char* id, const variants& elements, int count) {
	char temp[260]; stringbuilder sb(temp);
	answers an; add_elements(an, elements);
	if(szstart(id, "Choose"))
		sb.add(getnm(id));
	else {
		sb.add("%Choose [%1]", getnm(id));
		if(count > 0)
			sb.adds("(%-Left %1i)", count);
	}
	return an.choose(temp);
}

static void apply_advance(const advancei& e) {
	pushvalue push_modifier(modifier, Temporary);
	pushvalue push_permanent(permanent_modifier, true);
	if(e.choose) {
		auto count = e.choose;
		while(count-- > 0) {
			modifier = Temporary;
			permanent_modifier = true;
			auto v = choose_advance(e.id, e.elements, count + 1);
			if(e.object.iskind<script>()) {
				pushvalue push(last_result, v);
				bsdata<script>::elements[e.object.value].proc(0, 0);
			} else
				script::run(v);
		}
	} else
		script::run(e.elements);
}

static void apply_abilities(adat<char, 6>& source, const listi* abilitites) {
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
	auto pv = (abilityi*)an.choose(temp);
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
	adat<char, 6> abilities;
	abilities.add(15);
	abilities.add(14);
	abilities.add(13);
	abilities.add(12);
	abilities.add(11);
	abilities.add(9);
	while(abilities.getcount() > 0)
		apply_abilities(abilities, (listi*)list.getpointer());
}

void character::advance(variant object, int level) {
	pushvalue push_player(player, this);
	for(auto& e : bsdata<advancei>()) {
		if(e.object == object && e.level == level)
			apply_advance(e);
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
	apply_abilities(this);
}