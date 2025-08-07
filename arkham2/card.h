#pragma once

#include "flagable.h"
#include "nameable.h"
#include "variant.h"

enum cardt : unsigned char {
	Ally, Arkham, CommonItem, Gate, Monster, Myth, OtherWorld, Skill, Special, Spell, Street, UniqueItem,
};

struct cardti : nameable {
};
struct cardi : nameable {
	cardt		type; // Card deck or card type
	char		count; // Total card count in deck
	variants	use; // Card use script if apply
	variants	wear; // Card wear script if apply
	flag32		tags; // Any card tags
};
struct card {
	short unsigned type;
	short unsigned exhaused : 1;
	short unsigned marker : 3;
};