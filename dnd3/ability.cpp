#include "ability.h"
#include "list.h"
#include "pushvalue.h"
#include "script.h"

const int ability_maximum = 64;

int ability_result;

BSDATAC(abilityi, ability_maximum)

static void calculate(statable* p, variants source);

static void calculate(statable* p, variant v) {
	if(v.iskind<abilityi>()) {
		if(v.counter)
			ability_result += p->abilities[v.value] * v.counter;
		else
			ability_result += p->abilities[v.value];
	} else if(v.iskind<listi>())
		calculate(p, bsdata<listi>::elements[v.value].elements);
	else if(v.iskind<script>())
		bsdata<script>::elements[v.value].proc(v.counter);
}

static void calculate(statable* p, variants source) {
	ability_result = 0;
	for(auto v : source)
		calculate(p, v);
}

void statable::update() {
	for(auto i = 0; i < ability_maximum; i++) {
		auto& e = bsdata<abilityi>::elements[i];
		if(e.formula) {
			calculate(this, e.formula);
			abilities[i] += ability_result;
		}
	}
}