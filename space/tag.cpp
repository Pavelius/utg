#include "main.h"

BSDATA(tagi) = {
	{"BadSpot"}, // ����� -1 �� ���� ��������� ������ �� ����� �����
	{"Opportunity"}, // ����� +1 �� ���� ��������� ������ �� ����� �����
	{"Armored"},
	{"Explosion"},
	{"HeavyWeapon"},
	{"Smashing"},// Inflict gear as additional harm
	{"Tricky"}, // Inflict exhause as additional harm
	{"Agile"}, // +1 exhause
	{"Tought"},
	{"Leader"},
	{"Regular"},
	{"Veteran"},
	{"Elite"},
	{"Rifle"},
	{"Vehicle"},
	{"Broken"},
	{"Covered"},
	{"Illuminated"},
	{"Hiding"}, // ����� �� ����.
	{"Intimate"},
	{"Close"},
	{"Far"},
};
assert_enum(tagi, Far)