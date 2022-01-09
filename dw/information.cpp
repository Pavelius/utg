#include "main.h"

static void addn(stringbuilder& sb, const char* id, int v) {
	if(!v)
		return;
	if(sb)
		sb.add(",");
	sb.adds("%1 %2i", getnm(id), v);
}

static void addn(stringbuilder& sb, dice v) {
	if(v.c)
		sb.add("%1id%2i", v.c, v.d);
	if(v.b)
	sb.add("%+1i", v.b);
}

static void addn(stringbuilder& sb, const char* id, dice v) {
	if(!v)
		return;
	if(sb)
		sb.add(",");
	sb.adds(getnm(id));
	sb.add(" ");
	addn(sb, v);
}

void packi::getinfo(stringbuilder& sb) const {
	auto m = elements.count;
	variant last = 0;
	int count = 1;
	auto pbg = sb.get(); pbg[0] = 0;
	auto pse = elements.begin();
	for(unsigned i = 0; i < m; i++) {
		auto v = pse[i];
		if(i != (m - 1) && v == pse[i + 1]) {
			count++;
			continue;
		}
		last = v;
		if(pbg[0]) {
			if(i == (m - 1))
				sb.add(" и ");
			else
				sb.add(", ");
		}
		if(count == 1)
			sb.add(v.getname());
		else
			sb.add("%1i %-2", count, v.getname());
		count = 1;
	}
}

void creature::getinfo(stringbuilder& sb) const {
	sb.add("%1-%2", getname(), getnm(bsdata<classi>::get(type).id));
	sb.adds("(");
	sb.add("’иты %1i/%2i", get(HP), hp);
	addn(sb, "Armor", get(Armor));
	sb.add(")");
}

void creature::getinfo(const void* object, variant v, stringbuilder& sb) {
	auto p = (creature*)object;
	switch(v.type) {
	case Ability:
		switch(v.value) {
		case Strenght:
		case Dexterity:
		case Constitution:
		case Intellegence:
		case Wisdow:
		case Charisma:
			sb.add("[%1i] \t%+2i", p->abilities[v.value], p->getbonus((ability_s)v.value)); break;
			break;
		case Damage:
			sb.add("[");
			addn(sb, p->getdamage());
			sb.add("]");
			break;
		default:
			sb.add("[%1i]", p->abilities[v.value]); break;
			break;
		}
	default: break;
	}
}