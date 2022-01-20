#include "main.h"

BSDATA(tagi) = {
	{"Explosion", {Gear}},
	{"HeavyWeapon", {Gear}},
	{"Tought", {Gear}},
	{"Regular", {Injury, Gear}},
	{"Veteran", {Injury, Morale}},
	{"Elite", {Injury, Gear}},
	{"Vehicle", {Armor}},
	{"BadSpot"}, // ����� -1 �� ���� ��������� ������ �� ����� �����
	{"Opportunity"}, // ����� +1 �� ���� ��������� ������ �� ����� �����
	{"Hiding"}, // ����� �� ����.
	{"Covered"},
	{"Illuminated"},
	{"Intimate"},
	{"Close"},
	{"Far"},
};
assert_enum(tagi, Far)