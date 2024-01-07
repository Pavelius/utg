#include "ability.h"
#include "crt.h"
#include "list.h"
#include "pushvalue.h"
#include "script.h"
#include "statable.h"
#include "numberlist.h"

#define ISU(F, V) ((F & FG(V))!=0)

int ability_result;
int result_param[4];

void calculate(statable* p, variants source);

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

void calculate(statable* p, variants source) {
	pushvalue push_b(script_begin, source.begin());
	pushvalue push_e(script_end, source.end());
	while(script_begin < script_end) {
		calculate(p, *script_begin);
		script_begin++;
	}
}

void statable::update() {
	for(auto i = 0; i < sizeof(abilities) / sizeof(abilities[0]); i++) {
		auto& e = bsdata<abilityi>::elements[i];
		if(e.formula) {
			pushvalue push(ability_result, 0);
			calculate(this, e.formula);
			abilities[i] += ability_result;
		}
	}
}

void statable::creating() {
	for(auto i = 0; i < sizeof(abilities) / sizeof(abilities[0]); i++) {
		auto& e = bsdata<abilityi>::elements[i];
		if(e.creating) {
			pushvalue push(ability_result, 0);
			calculate(this, e.creating);
			abilities[i] += ability_result;
		}
	}
	for(auto i = 0; i < sizeof(abilities) / sizeof(abilities[0]); i++) {
		auto& e = bsdata<abilityi>::elements[i];
		if(!abilities[i])
			continue;
		if(e.formula) {
			pushvalue push(ability_result, 0);
			calculate(this, e.formula);
			abilities[i] -= ability_result;
		}
	}
}

void statable::copy(const statable& v) {
	*this = v;
	//memcpy(this, &v, sizeof(*this));
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