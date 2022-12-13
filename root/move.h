#include "ability.h"
#include "flagable.h"

#pragma once

enum move_s : unsigned char {
	Acrobatics, Blindside, Counterfeit, DisableDevice, Hide,
	PickLock, PickPocket, SleightOfHand, Sneak,
	FigureSomeoneOut, PersuadeNPC,
	ReadTenseSituation, TrickNPC, TrustFate, WreckSomething,
	EngageMelee, GrappleEnemy, TargetSomeone,
	Cleave, ConfuseSenses, Disarm,
	HarryGroup, ImproviseWeapon, Parry,
	QuickShot, StormGroup, TrickShot, ViciousStrike,
	AskFavor, MeetSomeoneImportant, DrawAttention, SwayNPC, MakePointedThread, CommandResources,
};
enum move_mechanic_s : unsigned char {
	PlainMove,
	Choose1, Choose1or2, Choose1or3, Choose2or3,
	ChooseYouAndEnemy,
};
enum move_type_s : unsigned char {
	BasicMove, RoguishFeat, WeaponSkill, ReputationMove, TravelMove,
};
typedef flagable<1 + CommandResources / 8> movea;
struct movei {
	const char* id;
	move_type_s type;
	ability_s roll;
	move_mechanic_s	mechanic;
};
