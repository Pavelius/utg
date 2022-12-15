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

int creature::getmaximumhp() const {
	return get(Constitution) + geti().damage;
}

void creature::getinfo(stringbuilder& sb) const {
	sb.add("%1-%2", getname(), getnm(bsdata<classi>::get(type).id));
	sb.adds("(");
	sb.add("Хиты %1i/%2i", get(HP), getmaximumhp());
	addn(sb, "Armor", get(Armor));
	sb.add(")");
}

void creature::getpropertyst(const void* object, variant v, stringbuilder& sb) {
	auto p = (creature*)object;
	if(v.iskind<abilityi>()) {
		switch(v.value) {
		case Strenght:
		case Dexterity:
		case Constitution:
		case Intellegence:
		case Wisdow:
		case Charisma:
			sb.add("[%1i] \t%+2i", p->abilities[v.value], p->getbonus((ability_s)v.value));
			break;
		case Damage:
			sb.add("[");
			addn(sb, p->getdamage());
			sb.add("]");
			break;
		default:
			if(p->abilities[v.value])
				sb.add("[%1i]", p->abilities[v.value]);
			break;
		}
	}
}