#include "ability.h"
#include "alignment.h"
#include "character.h"
#include "class.h"
#include "damage.h"
#include "item.h"
#include "modifier.h"
#include "race.h"
#include "script.h"
#include "skill.h"

variant last_result;

void apply_modifier(modifier_s v, int bonus) {
	switch(v) {
	case Permanent: permanent_modifier = true; break;
	case Temporary: permanent_modifier = false; break;
	default: modifier = v; break;
	}
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
		default: break;
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
	}
}

static void character_generate(int bonus, int param) {
	if(last_result.iskind<racei>())
		player->setkind(last_result);
	else if(last_result.iskind<alignmenti>())
		player->alignment = (char)last_result.value;
	else if(last_result.iskind<genderi>())
		player->setgender((gender_s)last_result.value);
	else if(last_result.iskind<classi>()) {
		player->classes[last_result.value]++;
		player->advance(last_result, player->classes[last_result.value]);
	}
}

BSDATA(script) = {
	{"CharacterGenerate", character_generate},
	{"ChooseElement"},
};
BSDATAF(script)