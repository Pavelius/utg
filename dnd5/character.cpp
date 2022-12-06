#include "answers.h"
#include "advance.h"
#include "character.h"
#include "list.h"
#include "modifier.h"
#include "pushvalue.h"
#include "race.h"
#include "script.h"

character *player;

void apply_modifier(modifier_s v, int bonus);

static void add_elements(answers& an, const variants& elements) {
	for(auto v : elements) {
		if(v.iskind<listi>())
			add_elements(an, bsdata<listi>::elements[v.value].elements);
		else if(v.iskind<modifieri>())
			apply_modifier((modifier_s)v.value, v.counter);
		else
			an.add(v.getpointer(), v.getname());
	}
}

static variant choose_advance(const advancei& source) {
	pushvalue push_modifier(modifier, Temporary);
	pushvalue push_permanent(permanent_modifier, true);
	char temp[260]; stringbuilder sb(temp);
	answers an; add_elements(an, source.elements);
	auto pn = source.id;
	if(szstart(pn, "Choose"))
		pn = getnm(pn);
	else {
		sb.clear();
		sb.add("%Choose [%1]", getnm(source.id));
		pn = temp;
	}
	return an.choose(pn);
}

static void apply_advance(const advancei& e) {
	pushvalue push_modifier(modifier, Temporary);
	pushvalue push_permanent(permanent_modifier, true);
	if(e.choose) {
		auto v = choose_advance(e);
		if(e.object.iskind<script>()) {
			pushvalue push(last_result, v);
			bsdata<script>::elements[e.object.value].proc(0, 0);
		}
	} else
		script::run(e.elements);
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
}