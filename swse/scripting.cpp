#include "ability.h"
#include "creature.h"
#include "list.h"
#include "modifier.h"
#include "pushvalue.h"
#include "script.h"

void runscript(variant v) {
	if(v.iskind<modifieri>())
		modifier = (modifier_s)v.value;
	else if(v.iskind<listi>())
		runscript(bsdata<listi>::elements[v.value].elements);
	else if(v.iskind<script>())
		bsdata<script>::elements[v.value].proc(v.counter, bsdata<script>::elements[v.value].param);
	else if(v.iskind<abilityi>()) {
		switch(modifier) {
		case Permanent: player->basic.abilities[v.value] += v.counter; break;
		default: player->abilities[v.value] += v.counter; break;
		}
	}
}

void runscript(const variants& elements) {
	pushvalue push_modifier(modifier, NoModifier);
	for(auto v : elements)
		runscript(v);
}

void initialize_script() {
	script::prun = runscript;
}

static void choose_creature(int bonus, int param) {
	player = creatures.choose(0, getnm("Cancel"));
}

BSDATA(script) = {
	{"ChooseCreature", choose_creature},
};
BSDATAF(script)