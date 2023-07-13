#include "ability.h"
#include "flagable.h"
#include "variant.h"

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
enum move_type_s : unsigned char {
	BasicMove, RoguishFeat, WeaponSkill, ReputationMove, TravelMove,
};
typedef flagable<1 + CommandResources / 8> movea;
struct moveoptioni {
	short unsigned	index, next;
	const char*		text;
	variants		effect;
	void			clear() { memset(this, 0, sizeof(*this)); }
	void			getinfo(stringbuilder& sb) const;
	bool			isanswer() const { return index == 0xFFFF; }
};
struct movei {
	typedef void(*fnproc)();
	const char*		id;
	move_type_s		type;
	ability_s		roll;
	sliceu<moveoptioni> options;
	static int		choose_count;
	const moveoptioni* choose(const moveoptioni* p) const;
	const moveoptioni* findprompt(short unsigned result) const;
	const moveoptioni* getanswer(const moveoptioni* p) const;
	static void		read(const char* url);
	void			run() const;
};
extern const movei* lastmove;