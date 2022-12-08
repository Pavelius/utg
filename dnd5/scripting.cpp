#include "ability.h"
#include "alignment.h"
#include "character.h"
#include "class.h"
#include "collection.h"
#include "damage.h"
#include "item.h"
#include "modifier.h"
#include "race.h"
#include "script.h"
#include "skill.h"
#include "spell.h"

variant last_result;

void apply_modifier(modifier_s v, int bonus) {
	switch(v) {
	case Permanent: permanent_modifier = true; break;
	case Temporary: permanent_modifier = false; break;
	default: modifier = v; break;
	}
}

static void item_add(int type) {
	item m;
	m.clear();
	m.type = type;
	player->additem(m);
}

void main_script(variant v) {
	if(v.iskind<modifieri>())
		apply_modifier((modifier_s)v.value, v.counter);
	else if(v.iskind<abilityi>()) {
		switch(modifier) {
		case Proficient: player->saves.set(v.value, v.counter >= 0); break;
		default: player->abilitites[v.value] += v.counter; break;
		}
	} else if(v.iskind<itemi>()) {
		switch(modifier) {
		case Proficient: player->items.set(v.value, v.counter >= 0); break;
		default: item_add(v.value); break;
		}
	} else if(v.iskind<damagei>()) {
		switch(modifier) {
		case Resist: player->resist.set(v.value, v.counter >= 0); break;
		case Immunity: player->immunity.set(v.value, v.counter >= 0); break;
		case Vulnerable: player->vulnerable.set(v.value, v.counter >= 0); break;
		default: break;
		}
	} else if(v.iskind<skilli>()) {
		switch(modifier) {
		case Proficient: player->skills.set(v.value, v.counter >= 0); break;
		default: break;
		}
	} else if(v.iskind<spelli>()) {
		switch(modifier) {
		case Proficient: player->spells_knows.set(v.value, v.counter >= 0); break;
		default: break;
		}
	}
}

bool script::isallow(variant v) {
	if(v.iskind<itemi>()) {
		switch(modifier) {
		case Allowed: return player->items.is(v.value) != (v.counter >= 0);
		case Proficient: return player->items.is(v.value) != (v.counter >= 0);
		default: break;
		}
	} else if(v.iskind<damagei>()) {
		switch(modifier) {
		case Resist: return player->resist.is(v.value) != (v.counter >= 0);
		case Immunity: return player->immunity.is(v.value) != (v.counter >= 0);
		case Vulnerable: return player->vulnerable.is(v.value) != (v.counter >= 0);
		default: break;
		}
	} else if(v.iskind<skilli>()) {
		switch(modifier) {
		case Proficient: return player->skills.is(v.value) != (v.counter >= 0);
		default: break;
		}
	} else if(v.iskind<spelli>()) {
		switch(modifier) {
		case Proficient: return player->spells_knows.is(v.value) != (v.counter >= 0);
		default: break;
		}
	}
	return true;
}

static bool race_parent(const void* object, int param) {
	return ((racei*)object)->parent == (racei*)param;
}

static const racei* choose_subrace(const racei* p) {
	collection<racei> source;
	source.select();
	source.match(race_parent, (int)p, true);
	if(source)
		p = source.choose(getnm("ChooseSubRace"));
	return p;
}

static void character_generate(int bonus, int param) {
	if(last_result.iskind<racei>()) {
		const racei* p = bsdata<racei>::elements + last_result.value;
		p = choose_subrace(p);
		player->setkind(p);
	} else if(last_result.iskind<alignmenti>())
		player->alignment = (char)last_result.value;
	else if(last_result.iskind<genderi>())
		player->setgender((gender_s)last_result.value);
	else if(last_result.iskind<classi>())
		player->classes[last_result.value]++;
}

BSDATA(script) = {
	{"CharacterGenerate", character_generate},
	{"ChooseElement"},
};
BSDATAF(script)