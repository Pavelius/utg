#include "ability.h"
#include "advance.h"
#include "answers.h"
#include "consumable.h"
#include "collection.h"
#include "creature.h"
#include "list.h"
#include "material.h"
#include "modifier.h"
#include "pushvalue.h"
#include "roll.h"
#include "script.h"
#include "statable.h"

static const char* last_id;
static int choose_count;

int calculate(statable* p, variants source);

static int get_bonus(int value) {
	return value ? value : 1;
}

static int get_value(int value) {
	switch(value) {
	case 101: return result_value;
	case -101: return -result_value;
	default: return value;
	}
}

template<> void fnscript<abilityi>(int index, int value) {
	switch(modifier) {
	case Current: player->abilities[index] += get_value(value); break;
	case Permanent: player->basic.abilities[index] += get_value(value); break;
	case Calculation: result_value += player->abilities[index] * get_bonus(value); break;
	}
}

template<> void fnscript<consumablei>(int index, int value) {
	player->consumables[index] += value;
}

template<> void fnscript<materiali>(int index, int value) {
	if(value >= 0) {
		switch(modifier) {
		case Resist: player->resistance |= FG(index); break;
		case Immune: player->immunity |= FG(index); break;
		case Vulnerable: player->vulnerable |= FG(index); break;
		}
	} else {
		switch(modifier) {
		case Resist: player->resistance &= ~FG(index); break;
		case Immune: player->immunity &= ~FG(index); break;
		case Vulnerable: player->vulnerable &= ~FG(index); break;
		}
	}
}

template<> void fnscript<rolli>(int index, int value) {
	result_value += bsdata<rolli>::elements[index].roll() + value;
}

template<> void fnscript<monsteri>(int index, int value) {
}

static int getmaximum(variant v) {

}

static void add_random(int counter) {
	if(!counter)
		return;
	result_value += xrand(0, counter);
}

static void number(int counter) {
	result_value += counter;
}

static void divide(int counter) {
	if(counter)
		result_value /= counter;
}

static void multiply(int counter) {
	if(counter)
		result_value *= counter;
}

static void set_param(int counter) {
	if(counter < sizeof(result_param) / sizeof(result_param[0]))
		result_param[counter] = result_value;
}

static bool in_party(const creature* p) {
	for(auto pv : party) {
		if(pv == p)
			return true;
	}
	return false;
}

static void shrink_party() {
	auto ps = party;
	auto pb = party;
	auto pe = pb + sizeof(party) / sizeof(party[0]);
	for(; pb < pe; pb++) {
		if(!(*pb))
			continue;
		*ps++ = *pb;
	}
	while(ps < pe)
		*ps++ = 0;
}

void join_party(int bonus) {
	if(bonus >= 0) {
		if(in_party(player))
			return;
		for(auto& pv : party) {
			if(!pv) {
				pv = player;
				return;
			}
		}
	} else {
		for(auto& pv : party) {
			if(pv == player) {
				pv = 0;
				shrink_party();
				return;
			}
		}
	}
}

static void apply_choose(variant v, variant type) {
	if(type.iskind<enumgroupi>())
		player->values[type.value] = v.value;
	else if(type.iskind<abilityi>()) {
		switch(modifier) {
		case Permanent: player->basic.abilities[type.value] = (char)v.value; break;
		case Current: player->abilities[type.value] = (char)v.value; break;
		}
	}		
}

static void add_querry(variant type) {
	if(type.iskind<enumgroupi>()) {
		for(auto& e : bsdata<enumi>()) {
			if(e.type == type)
				an.add(&e, e.getname());
		}
	} else if(type.iskind<listi>()) {
		for(auto v : bsdata<listi>::elements[type.value].elements) {
			if(v.iskind<listi>())
				add_querry(v);
			else
				an.add(v.getpointer(), v.getname());
		}
	}
}

static void apply_select(int bonus) {
	an.clear();
	add_querry(*script_begin++);
}

static void random_abilities(int bonus) {
	auto pa = bsdata<listi>::find("PlayerAbilities");
	if(!pa)
		return;
	auto pr = bsdata<listi>::find("PlayerAbilitiesRoll");
	if(!pr)
		return;
	for(auto v : pa->elements)
		player->basic.abilities[v.value] = calculate(player, pr->elements);
}

static void* choose_prompt(const char* id) {
	if(choose_count > 1) {
		char temp[260]; stringbuilder sb(temp);
		sb.add(getnm(id));
		sb.add(getnm("ChooseLeft"), choose_count);
		return an.choose(temp, 0);
	} else
		return an.choose(getnm(id), 0);
}

void* choose_value(const char* id) {
	auto push_column = answers::column_count;
	answers::column_count = -1;
	auto result = choose_prompt(id);
	answers::column_count = push_column;
	return result;
}

static void apply_choose(variant v) {
	if(v.iskind<enumi>()) {
		auto p = bsdata<enumi>::elements + v.value;
		player->values[p->type.value] = p->index;
	}
}

static void choose(int bonus) {
	if(!bonus)
		bonus = 1;
	auto push_choose = choose_count;
	choose_count = bonus;
	while(choose_count > 0) {
		auto p = choose_value(last_id);
		if(p) {
			apply_choose(p);
			auto i = an.indexof(p);
			if(i != -1)
				an.remove(i);
		}
		choose_count--;
	}
	choose_count = push_choose;
}

static void apply_advance(variant type) {
	pushvalue push_id(last_id);
	pushvalue push_value(result_value);
	pushvalue push_modifier(modifier);
	for(auto& e : bsdata<advancementi>()) {
		if(e.type == type) {
			last_id = e.id;
			result_value = 0;
			modifier = Permanent;
			script_run(e.elements);
		}
	}
}

void creature::advance(const char* id, int bonus) {
	variant type = id;
	if(!type)
		return;
	type.counter = bonus;
	apply_advance(type);
}

static void minimum_value(int bonus) {
	if(result_value < bonus)
		result_value = bonus;
}

BSDATA(script) = {
	{"AddRandom", add_random},
	{"Choose", choose},
	{"Divide", divide},
	{"JoinParty", join_party},
	{"Minimum", minimum_value},
	{"Multiply", multiply},
	{"Number", number},
	{"RandomAbilities", random_abilities},
	{"SetParam", set_param},
	{"Select", apply_select},
};
BSDATAF(script)