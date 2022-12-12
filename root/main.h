#include "ability.h"
#include "bonus.h"
#include "chooselist.h"
#include "harmable.h"
#include "message.h"
#include "menu.h"
#include "nameable.h"
#include "quest.h"
#include "result.h"
#include "script.h"
#include "tag.h"
#include "utg.h"

#pragma once

enum ability_s : unsigned char {
	Charm, Cunning, Finess, Luck, Might
};
enum special_s : unsigned char {
	Choose, Harm, Range, Ignore,
};
enum tag_s : unsigned char {
	Blunted, Cumbersome, Flexible, Large, Makeshift, MetalBoss,
	Precise, Quick, Reach, Reinforced, Sharp, SteelOfRabbitfolk, SteelOfFoxfolk,
	TightlyWoven, Throwable, Unassuming, Weighty,
	Intimate, Close, Far,
};
enum move_type_s : unsigned char {
	BasicMove, RoguishFeat, WeaponSkill, ReputationMove, TravelMove,
};
enum move_mechanic_s : unsigned char {
	PlainMove,
	Choose1, Choose1or2, Choose1or3, Choose2or3,
	ChooseYouAndEnemy,
};
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
typedef flagable<1 + CommandResources / 8> movea;
typedef char choosea[StrongSuccess + 1];
struct statable {
	char			abilities[Might + 1];
	int				get(ability_s v) const { return abilities[v]; }
};
struct riski {
	const char*		id;
	int				page;
};
struct movei {
	const char*		id;
	move_type_s		type;
	ability_s		roll;
	move_mechanic_s	mechanic;
};
struct speciali {
	const char*		id;
};
struct tagable {
	flaga			tags;
	movea			moves;
	void			add(tag_s v) { tags.set(v); }
	void			add(move_s v) { moves.set(v); }
	constexpr bool	is(tag_s v) const { return tags.is(v); }
	constexpr bool	is(move_s v) const { return moves.is(v); }
};
struct itemi : nameable, tagable {
	char			wear;
	constexpr bool	isweapon() const { return is(Far) || is(Close) || is(Intimate); }
};
struct item {
	unsigned char	type;
	unsigned char	wear : 3;
	constexpr explicit operator bool() const { return type != 0; }
	const itemi&	geti() const { return bsdata<itemi>::elements[type]; }
};
struct rangeable {
	tag_s			range = Close;
	void			addrange(int v);
	void			setrange(tag_s v) { range = v; }
};
struct creature : nameable, harmable, statable, rangeable {
	harmable		suffer;
	void			roll(move_s v);
	void			move(move_s v);
};
namespace draw {
void				information(const char* format, ...);
void				warning(const char* format, ...);
}