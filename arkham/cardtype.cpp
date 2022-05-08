#include "main.h"

BSDATA(cardtypei) = {
	{"Ally"},
	{"Arkham"},
	{"CommonItem"},
	{"Gate"},
	{"Monster"},
	{"Myth"},
	{"OtherWorld"},
	{"Skill"},
	{"Special"},
	{"Spell"},
	{"Street"},
	{"UniqueItem"},
};
assert_enum(cardtypei, UniqueItem)

int	cardprotoi::getcost(int discount) const {
	int v = cost + discount;
	if(v < 0)
		v = 0;
	if(cost > 0 && v == 0)
		v = 1;
	return v;
}

int cardprotoi::getcombat(const cardprotoi& enemy) const {
	switch(special) {
	case PhysicalWeapon:
		if(enemy.is(PhysicalImmunity))
			break;
		if(enemy.is(PhysicalResistance))
			return bonus / 2;
		else
			return bonus;
		break;
	case MagicalWeapon:
		if(enemy.is(MagicalImmunity))
			break;
		if(enemy.is(MagicalResistance))
			return bonus / 2;
		else
			return bonus;
		break;
	}
	return 0;
}