#include "ability.h"
#include "list.h"
#include "pushvalue.h"
#include "script.h"
#include "statable.h"
#include "numberlist.h"

const int ability_maximum = 64;

variant* script_begin;
variant* script_end;

int ability_result;
int result_param[4];

BSDATAC(abilityi, ability_maximum)

static void calculate(statable* p, variants source);

static void calculate(statable* p, variant v) {
	if(v.iskind<abilityi>()) {
		if(!v.counter)
			v.counter = 1;
		ability_result += p->abilities[v.value] * v.counter;
	} else if(v.iskind<listi>()) {
		if(!v.counter)
			v.counter = 1;
		auto push_result = ability_result; ability_result = 0;
		calculate(p, bsdata<listi>::elements[v.value].elements);
		ability_result = push_result + ability_result * v.counter;
	} else if(v.iskind<script>())
		bsdata<script>::elements[v.value].proc(v.counter);
	else if(v.iskind<numberlist>()) {
		if(!v.counter)
			v.counter = 1;
		ability_result = bsdata<numberlist>::elements[v.value].match(ability_result) * v.counter;
	}
}

static void calculate(statable* p, variants source) {
	pushvalue push_b(script_begin, source.begin());
	pushvalue push_e(script_end, source.end());
	while(script_begin < script_end) {
		calculate(p, *script_begin);
		script_begin++;
	}
}

void statable::update() {
	for(auto i = 0; i < ability_maximum; i++) {
		auto& e = bsdata<abilityi>::elements[i];
		if(e.formula) {
			pushvalue push(ability_result, 0);
			calculate(this, e.formula);
			abilities[i] += ability_result;
		}
	}
}