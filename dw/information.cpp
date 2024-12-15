#include "class.h"
#include "creature.h"
#include "script.h"

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

template<> void ftinfo<creature>(const void* object, stringbuilder& sb) {
	auto p = (creature*)object;
	sb.add("%1-%2", p->getname(), getnm(p->geti().id));
	sb.adds("(");
	sb.add("Хиты %1i/%2i", p->get(HP), p->getmaximumhp());
	addn(sb, "Armor", p->get(Armor));
	sb.add(")");
}

void creature::getpropertyst(const void* object, variant v, stringbuilder& sb) {
	auto p = (creature*)object;
	int n;
	if(v.iskind<abilityi>()) {
		switch(v.value) {
		case Strenght: case Dexterity: case Constitution:
		case Intellegence: case Wisdow: case Charisma:
			sb.add("%+1i", p->abilities[v.value]);
			break;
		case Damage:
			addn(sb, p->getdamage());
			break;
		default:
			if(p->abilities[v.value])
				sb.add("%1i", p->abilities[v.value]);
			break;
		}
	} else if(v.iskind<tagi>()) {
		switch(v.value) {
		case Coins:
			sb.add("%1i", p->coins);
			break;
		}
	}
}