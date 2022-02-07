#include "cannoneer.h"
#include "charname.h"
#include "chooselist.h"
#include "counters.h"
#include "iterator.h"
#include "gender.h"
#include "group.h"
#include "message.h"
#include "pathfind.h"
#include "quest.h"
#include "recordset.h"
#include "tag.h"
#include "utg.h"

#pragma once

template<typename T> struct bsmeta;

enum ability_s : unsigned char {
	Exploration, Brawl, Hunting, Aim, Swagger, Navigation,
	Reroll, Misfortune, Treasure,
	Crew, Discontent, Supply, Hull,
	Danger, DangerMaximum,
	Threat, Mission, MissionMaximum, Cabine, CabineMaximum,
	Stars, Level, Infamy,
	Gun1, Gun2, Gun3, Gun4,
	Eat
};
enum special_s : unsigned char {
	Name, Nickname, NicknameEnd,
	Block, Choose, Roll, RollGuns, RollSilent, Damage, Bury, Scout, Skill, Scene,
	Tile000, Tile900, TileRock, AddTile, RemoveTile, SetShip, MoveToPlayer, ShowMap,
	FullThrottle, TradeFriend,
	EatSupply, ZeroSupplyIfNot, ZeroRerollIfNot,
	ZeroCounters, ZeroDanger, CounterName, ChooseCounter, ChooseCustom,
	ReloadGunOrHull, ReloadGun, UpgradeGun, AddGun, AddGunUnloaded,
	VisitManyTimes, VisitRequired, NotUseAction, IfChoosedAction, StopActions,
	CheckDanger, RemoveAllNavigation, PlayStars, Sail, LostGame, WinGame,
	Page000, Page100, Page200, Page300, Page400, Page500, Page600, Page700, Page800, Page900,
	PageForward, PageNext,
	CounterA, CounterB, CounterC, CounterD, CounterX,
	PenaltyA, PenaltyB, PenaltyC, PenaltyD,
	Entry, MarkEntry, MarkVisit, SetVisit,
	BonusToAll,
	IfEqual, IfZeroForward, IfNonZeroForward, IfNotSail, IfLast,
};
enum tag_s : unsigned char {
	NoDigging, NoSteal, Valuable, Discard,
	Discarded,
};
enum trigger_s : unsigned char {
	NoTrigger,
	WhenChooseSpecial, WhenRoll, WhenThread, WhenAbilityIncreased, WhenAbilityDecreased,
	WhenUse,
};
enum variant_s : unsigned char {
	NoVariant,
	Ability, Card, Class, Condition, Gender, Goal, Group, Menu, NavigationTile,
	Quest, Special, Tag, Trigger, Value, Widget
};
enum abilityf_s : unsigned char {
	Negative,
};
enum answer_s {
	AnswerChoose = 0, AnswerPage = 30,
	AnswerForward = 1000,
	AnswerName = 6000,
	AnswerEntry = 6100,
	AnswerCustom = 6200,
	AnswerStartGame = 6500,
};

typedef flagable<8> taga;
typedef adat<const void*, 32> useda;
const int player_count = 4;
const indext map_x = 7;
const indext map_y = 6;

struct abilityi {
	const char*		id;
	unsigned		flags;
	constexpr bool	is(abilityf_s v) const { return (flags & FG(v)) != 0; }
};
struct tilei {
	short unsigned	frame;
	int				param;
	short unsigned	index;
	flagable<4>		tags;
	void			discard();
	static tilei*	findindex(indext v);
	static tilei*	find(int param);
	constexpr bool	is(tag_s v) const { return tags.is(v); }
	constexpr bool	isactive() const { return index!=0xFFFF; }
	constexpr bool	isblocked() const { return param== 0xFFFF; }
	constexpr bool	isnavigation() const { return param >= 1 && param <= 30; }
	bool			moveto(indext goal, int bonus);
	static tilei*	pick(int param);
	static tilei*	pick();
	void			setindex(indext v) { index = v; }
};
class tilea : public adat<tilei*, 128> {
public:
	void			select(int from, int to);
};
class oceani {
	indext			marker;
public:
	void			addpossiblecourse() const;
	void			addpossiblemove(int range) const;
	indext			choose(const char* title) const;
	indext			chooseroute(const char* title, int range = 1) const;
	static void		createobjects();
	void			createselections(int from, int to) const;
	static indext	getindex(short x, short y) { return y * map_x + x; }
	static point	gethexsize();
	indext			getmarker() const { return marker; }
	static short	getx(indext i) { return i % map_x; }
	static short	gety(indext i) { return i / map_x; }
	static void		initialize();
	void			setmarker(indext v) { marker = v; }
	static void		showindecies();
	void			showseamap();
	static void		showsplash();
	bool			stepto(indext start, indext goal);
	static indext	to(indext i, int direction);
};
struct goali {
	const char*		id;
	int				danger;
	short			reach_location;
};
class npcname {
	short unsigned	nameid;
	short unsigned	nicknameid;
public:
	const char*		getname() const;
	void			getname(stringbuilder& sb) const;
	gender_s		getgender() const;
	void			randomname();
};
struct classi {
	const char*		id;
	variant			types[5];
	char			maximum[6];
	char			levelup[5];
	char			exploration[4], brawl[4], hunting[4], aim[4], swagger[4], navigation[4];
	variants		event1, event2, event3, event4, event5;
	const char*		getearn(ability_s v) const;
};
class player : public npcname {
	static const int maxcount = 4;
	short unsigned	active;
	short unsigned	classid;
	variant			values[5];
	npcname			friends[maxcount - 1];
	unsigned char	order[maxcount];
	class string;
public:
	void			act(const char* format, ...) const { actn(utg::sb, format, xva_start(format)); }
	void			actn(stringbuilder& sb, const char* format, const char* format_param, bool add_sep = true) const;
	void			background() const;
	void			chooseclass();
	void			choosename();
	void			choosehistory();
	void			epilog(int level);
	void			generate();
	const npcname&	getactive() const { return active ? friends[(active - 1) % (maxcount - 1)] : *this; }
	const classi&	getclass() const { return bsdata<classi>::elements[classid]; }
	slice<npcname>	getfriends() { return slice<npcname>(friends); }
	variant			getvalue(int v) { return values[v % lenghtof(friends)]; }
	void			setorder(int v) { setplayer(order[v % maxcount]); }
	void			setplayer(int v) { active = v % maxcount; }
	void			shuffleparcipant();
};
struct speciali {
	const char*		id;
};
struct triggeri {
	const char*		id;
};
struct treasurei {
	const char*		id;
	char			abilities[6]; // Bonus to abilities
	trigger_s		trigger;
	ability_s		ability;
	char			bonus;
	flaga			tags;
	variants		gain, loss, use;
	short unsigned	owner;
	void			apply() const;
	static treasurei* find(const char* id);
	void			gaining() const;
	bool			is(tag_s v) const { return tags.is(v); }
	bool			isactive() const { return owner != 0xFFFF; }
	bool			isdiscardable() const;
	bool			isdiscarded() const { return is(Discarded); }
	void			lossing() const;
	static treasurei* pickvaluable();
	static treasurei* pickvaluable(int number);
	static treasurei* pickstory(int number);
	static void		prepare();
	void			setowner(const void* object);
	static void		sfgetinfo(const void* object, stringbuilder& sb);
	void			triggered();
};
class chest {
public:
	void			apply(trigger_s type, ability_s v);
	treasurei*		choosetreasure(const char* title, const char* cancel) const;
	void			gaintreasure(treasurei* pv);
	int				getbonus(ability_s v) const;
	int				gettreasurecount(tag_s v) const;
	void			losstreasure(treasurei* pv);
};
class pirate : public player, public chest {
	static const int max_treasures = 4;
	char			abilities[Infamy + 1];
	void			afterchange(ability_s v, int b);
	void			checkexperience(ability_s v);
	void			confirmroll();
	void			makeroll(special_s type);
	void			rolldices();
public:
	indext			actions[6];
	void			addaction(indext v);
	void			bury(int count);
	void			choosebonus(variant v1, variant v2);
	ability_s		chooseskill(const char* title) const;
	void			clear();
	void			clearactions();
	bool			confirm(ability_s v, int delta) const;
	void			gaintreasures(int count = 1);
	int				get(ability_s v) const { return abilities[v]; }
	static const char* getavatarst(const void* object);
	int				getmaximum(ability_s v) const;
	int				getnextstar(int value) const;
	bool			match(variant v) const;
	void			raiseskills(int count);
	void			roll(special_s type);
	static void		sfgetproperty(const void* object, variant v, stringbuilder& sb);
	void			set(ability_s v, int i);
	void			sortactions();
	void			tradefriend();
};
class shiplog {
	short unsigned	goal_id;
	bool			reach_location;
	taga			tags;
public:
	void			checkgoal();
	void			clear();
	static const quest* getentry(int v);
	static const char* getentryname(int v);
	static const char* getentrytext(int v);
	const goali*	getgoal() const { return goal_id == 0xFFFF ? 0 : bsdata<goali>::elements + goal_id; }
	bool			istag(int v) const;
	void			setgoal(const goali* v) { goal_id = bsdata<goali>::source.indexof(v); }
	void			settag(int v);
	void			removetag(short unsigned v) { tags.remove(v); }
};
class gamei : public pirate, public oceani, public cannoneer, public shiplog {
	char			scenario[32];
	flagable<2>		locked;
public:
	static void		apply(variant v);
	static void		background();
	static void		createtiles();
	void			createtreasure();
	void			clear();
	static void		choosecounter();
	void			chartacourse(int v);
	static int		getpage();
	void			information(const char* format, ...);
	void			information(ability_s v, int count);
	bool			ischoosed(int i) const;
	bool			islocked(int i) const { return locked.is(i); }
	void			lock(int i) { locked.set(i); }
	static void		script(int page);
	static void		showseamap();
	void			setgoal(const goali* v) {}
	static bool		sfapply(variant v, bool run) { if(run) apply(v); return true; }
	static void		sfgetinfo(const void* object, stringbuilder& sb);
	static void		sfgetstatus(const void* object, stringbuilder& sb);
	static void		sfgetproperty(const void* object, variant v, stringbuilder& sb);
	void			unlockall() { locked.clear(); }
	void			unlock(int i) { locked.remove(i); }
	void			warning(const char* format, ...);
	void			write(const char* url);
};
extern gamei		game;
extern int			last_result, last_roll, last_bonus;
extern counters		variables;
int					rollv(int bonus);
VKIND(gender_s, Gender)
VKIND(special_s, Special)
VKIND(treasurei, Card)
VKIND(trigger_s, Trigger)
VKIND(groupvaluei, Value)