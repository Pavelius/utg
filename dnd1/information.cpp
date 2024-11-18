#include "creature.h"

static void print(stringbuilder& sb, const interval& v) {
	if(!v.max)
		sb.add("%1i", v.max);
	else
		sb.add("%1i-%2i", v.min, v.max);
}

static void print_weight(stringbuilder& sb, int coins) {
	auto kg = coins / 20;
	auto gr = (coins % 20) * 5;
	sb.adds("%1i.%2.2i%Kg", kg, gr);
}

static void print_weight_hd(stringbuilder& sb, int coins) {
	if(!coins)
		return;
	sb.adds("%Weight");
	print_weight(sb, coins);
}

static void print_cost(stringbuilder& sb, int gp) {
	sb.adds("%1igp", gp);
}

static void print_cost_hd(stringbuilder& sb, int gp) {
	if(!gp)
		return;
	sb.adds("%Cost");
	print_cost(sb, gp);
}

void creature::getstatus(stringbuilder& sb) const {
	sb.add("%1 - %2 (%3i %-Level)", getname(),
		geti().getname(),
		get(Level));
//	if(is(Player))
		sb.adds("%-HP %1i/%2i", abilities[HP], abilities[HPMax]);
}

void creature::getstatus(const void* object, stringbuilder& sb) {
	auto index = bsdata<creature>::source.indexof(object);
	if(index == -1)
		return;
	auto p = bsdata<creature>::elements + index;
	if(p == object)
		p->getstatus(sb);
	else if(p->isitem(object))
		((item*)object)->getstatus(sb);
}

static void save_information(stringbuilder& sb, const creature* p, const slice<abilityn>& source) {
	auto ps = sb.get();
	for(auto v : source) {
		auto i = 20 - p->get(v);
		if(ps[0] != 0)
			sb.add("/");
		sb.add("%1i", i);
	}
}

void creature::getproperty(const void* object, variant id, stringbuilder& sb) {
	static abilityn saves[] = {SaveDeath, SaveWands, SaveParalize, SaveBreathWeapon, SaveSpells};
	auto p = (creature*)object;
	if(id.iskind<abilityi>()) {
		switch(id.value) {
		case MeleeDamage:
			print(sb, p->getdamage(MeleeWeapon));
			break;
		case HP:
			sb.add("%1i %-From %2i", p->abilities[HP], p->abilities[HPMax]);
			break;
		case MeleeToHit:
			sb.add("%1i", 19 - p->abilities[id.value]);
			break;
		case AC:
			sb.add("%1i", 9 - p->abilities[id.value]);
			break;
		case Strenght: case Dexterity: case Constitution:
		case Intellect: case Wisdow: case Charisma:
			sb.add("%1i", p->abilities[id.value]);
			break;
		case SaveSpells:
			save_information(sb, p, saves);
			break;
		default:
			sb.add("%1i", p->abilities[id.value]);
			break;
		}
	}
}

void item::getstatus(stringbuilder& sb) const {
	auto& ei = geti();
	print_weight_hd(sb, getweight());
	print_cost_hd(sb, getcost());
}