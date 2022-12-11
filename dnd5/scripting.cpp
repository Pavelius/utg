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

static void item_add(int type) {
	item m; m.clear();
	m.type = type;
	player->additem(m);
}

template<> void fnscript<modifieri>(int value, int bonus) {
	switch(value) {
	case NoModifier: permanent_modifier = false; modifier = NoModifier; break;
	case Permanent: permanent_modifier = true; break;
	default: modifier = (modifier_s)value; break;
	}
}

template<> void fnscript<abilityi>(int value, int bonus) {
	switch(modifier) {
	case Proficient: player->saves.set(value, bonus >= 0); break;
	default: player->abilitites[value] += bonus; break;
	}
}

template<> bool fntest<itemi>(int value, int bonus) {
	switch(modifier) {
	case Proficient: return player->items.is(value) != (bonus >= 0);
	default: return player->items.is(value) == (bonus >= 0);
	}
}
template<> void fnscript<itemi>(int value, int bonus) {
	switch(modifier) {
	case Proficient: player->items.set(value, bonus >= 0); break;
	default: item_add(value); break;
	}
}

template<> bool fntest<damagei>(int value, int bonus) {
	switch(modifier) {
	case Resist: return player->resist.is(value) != (bonus >= 0);
	case Immunity: return player->immunity.is(value) != (bonus >= 0);
	case Vulnerable: return player->vulnerable.is(value) != (bonus >= 0);
	default: return true;
	}
}
template<> void fnscript<damagei>(int value, int bonus) {
	switch(modifier) {
	case Resist: player->resist.set(value, bonus >= 0); break;
	case Immunity: player->immunity.set(value, bonus >= 0); break;
	case Vulnerable: player->vulnerable.set(value, bonus >= 0); break;
	default: break;
	}
}

template<> bool fntest<skilli>(int value, int bonus) {
	switch(modifier) {
	case Proficient: return player->skills.is(value) != (bonus >= 0);
	default: return true;
	}
}
template<> void fnscript<skilli>(int value, int bonus) {
	switch(modifier) {
	case Proficient: player->skills.set(value, bonus >= 0); break;
	default: break;
	}
}

template<> bool fntest<spelli>(int value, int bonus) {
	switch(modifier) {
	case Proficient: return player->spells_knows.is(value) != (bonus >= 0);
	default: return true;
	}
}
template<> void fnscript<spelli>(int value, int bonus) {
	switch(modifier) {
	case Proficient: player->spells_knows.set(value, bonus >= 0); break;
	default: break;
	}
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

static void character_generate(int bonus) {
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