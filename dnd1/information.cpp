#include "main.h"

static void print(stringbuilder& sb, const dice& v) {
	auto min = v.c + v.b;
	if(min < 1)
		min = 1;
	auto max = v.c * v.d + v.b;
	if(max < min)
		max = min;
	if(max == min)
		sb.add("%1i", max);
	else
		sb.add("%1i-%2i", min, max);
}

void creature::getstatus(stringbuilder& sb) const {
	sb.add("%1 - %2 (%3i %-Level)", getname(),
		getnm(bsdata<classi>::elements[type].id),
		get(Level));
}

void creature::getstatus(const void* object, stringbuilder& sb) {
	((creature*)object)->getstatus(sb);
}

void creature::getproperty(const void* object, variant id, stringbuilder& sb) {
	auto p = (creature*)object;
	if(id.iskind<abilityi>()) {
		switch(id.value) {
		case Damage:
			print(sb, p->getdamage(MeleeWeapon));
			break;
		case HP:
			sb.add("%1i %-From %2i", p->abilities[HP], p->abilities[HPMax]);
			break;
		case ToHit:
			sb.add("%1i", 19 - p->abilities[id.value] - p->abilities[MeleeToHit]);
			break;
		case AC:
			sb.add("%1i", 9 - p->abilities[id.value]);
			break;
		case Strenght: case Dexterity: case Constitution:
		case Intellect: case Wisdow: case Charisma:
			sb.add("%1i", p->abilities[id.value]);
			break;
		default:
			sb.add("%1i", p->abilities[id.value]);
			break;
		}
	}
}