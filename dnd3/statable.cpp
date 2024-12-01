#include "ability.h"
#include "flagable.h"
#include "list.h"
#include "modifier.h"
#include "pushvalue.h"
#include "roll.h"
#include "script.h"
#include "statable.h"
#include "numberlist.h"

#define ISU(F, V) ((F & FG(V))!=0)

int result_value;
int result_param[4];

int calculate(statable* p, variants source) {
	pushvalue push(result_value, 0);
	pushvalue push_b(script_begin, source.begin());
	pushvalue push_e(script_end, source.end());
	while(script_begin < script_end) {
		auto v = *script_begin;
		if(v.iskind<abilityi>()) {
			if(!v.counter)
				v.counter = 1;
			result_value += p->abilities[v.value] * v.counter;
		} else if(v.iskind<listi>()) {
			if(!v.counter)
				v.counter = 1;
			result_value += calculate(p, bsdata<listi>::elements[v.value].elements) * v.counter;
		} else if(v.iskind<formulai>()) {
			if(!v.counter)
				v.counter = 1;
			result_value += calculate(p, bsdata<formulai>::elements[v.value].formula) * v.counter;
		} else if(v.iskind<script>())
			bsdata<script>::elements[v.value].proc(v.counter);
		else if(v.iskind<rolli>())
			fnscript<rolli>(v.value, v.counter);
		else if(v.iskind<numberlist>()) {
			if(!v.counter)
				v.counter = 1;
			result_value = bsdata<numberlist>::elements[v.value].match(result_value) * v.counter;
		}
		script_begin++;
	}
	return result_value;
}

void statable::update() {
	for(auto i = 0; i < sizeof(abilities) / sizeof(abilities[0]); i++) {
		auto& e = bsdata<abilityi>::elements[i];
		if(e.formula)
			abilities[i] += calculate(this, e.formula);
	}
}

void statable::copy(const statable& v) {
	*this = v;
}

bool statable::isimmunity(int material) const {
	return ISU(immunity, material);
}

bool statable::isresistance(int material) const {
	return ISU(resistance, material);
}

bool statable::isvulnerable(int material) const {
	return ISU(vulnerable, material);
}

int	statable::getability(const char* id) const {
	auto pi = bsdata<abilityi>::find(id);
	if(!pi)
		return 0;
	return abilities[pi - bsdata<abilityi>::elements];
}

void statable::setability(const char* id, int value) {
	auto pi = bsdata<abilityi>::find(id);
	if(pi)
		abilities[pi - bsdata<abilityi>::elements] = value;
}