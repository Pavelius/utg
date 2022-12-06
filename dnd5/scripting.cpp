#include "ability.h"
#include "character.h"
#include "damage.h"
#include "item.h"
#include "modifier.h"
#include "script.h"
#include "skill.h"

void main_script(variant v) {
	if(v.iskind<abilityi>()) {
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
}

BSDATA(script) = {
	{"CharacterGenerate", character_generate},
	{"ChooseElement"},
};
BSDATAF(script)