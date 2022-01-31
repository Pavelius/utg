#include "cannoneer.h"
#include "charname.h"
#include "counters.h"
#include "gender.h"
#include "group.h"
#include "message.h"
#include "pathfind.h"
#include "quest.h"
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
	Tile000, Tile900, TileRock, AddTile, RemoveTile, SetShip,
	FullThrottle, TradeFriend,
	PaySupply, PaySupplyEat, ZeroSupplyOrDiscontent,
	ZeroCounters, CounterName, ChooseCounter,
	ReloadGunOrHull, ReloadGun, UpgradeGun, AddGun, AddGunUnloaded,
	VisitManyTimes, VisitRequired, CheckDanger, RemoveAllNavigation, PlayStars, Sail, LostGame, WinGame,
	Page000, Page100, Page200, Page300, Page400, Page500, Page600, Page700, Page800, Page900,
	CounterA, CounterB, CounterC, CounterD, CounterX,
	PenaltyA, PenaltyB, PenaltyC, PenaltyD,
	MarkEntry, IfCounterZero, IfExistEntry, IfSail,
};
enum tag_s : unsigned char {
	NoDigging, NoSteal, Usable,
};
enum event_s : unsigned char {
	WhenRoll, WhenRollOther, WhenDigging,
	WhenStartActingPhase, WhenActing, WhenUseReroll,
};
enum variant_s : unsigned char {
	NoVariant,
	Ability, Card, Class, Gender, Goal, Group, Menu, NavigationTile, Quest, Special, Tag, Value, Widget
};
enum ability_type_s : unsigned char {
	Positive, Negative,
};

typedef flagable<8> taga;
typedef short unsigned indext;
const int player_count = 4;

struct abilityi {
	const char*		id;
	ability_type_s	type;
};
struct tilei {
	indext			page, frame;
	static const tilei*	find(indext page);
};
class oceani {
	static const indext	mx = 7;
	static const indext	my = 6;
	indext			data[mx * my];
	indext			marker;
	void			blockwalls() const;
	void			blockopentiles() const;
	void			blockalltiles() const;
public:
	void			addpossiblecourse() const;
	void			addpossiblemove(int range) const;
	indext			choose(const char* title) const;
	indext			chooseroute(const char* title, int range = 1) const;
	void			createobjects() const;
	void			createselections(int from, int to) const;
	indext			getindex(const void* p) const;
	static indext	getindex(short x, short y) { return y * mx + x; }
	static point	gethexsize();
	indext			getlocation(indext i) const { return data[i]; }
	indext			getmarker() const { return marker; }
	static short	getx(indext i) { return i % mx; }
	static short	gety(indext i) { return i / mx; }
	static void		initialize();
	bool			isblocked(indext i) const { return ispassabletile(data[i]); }
	static bool		ispassabletile(indext v) { return v != pathfind::Blocked; }
	void			setlocation(indext i, int v) { data[i] = v; }
	void			setmarker(indext v) { marker = v; }
	static void		showindecies();
	void			showseamap();
	void			showsplash();
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
	void			clear();
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
	const char*		getearn(ability_s v) const;
};
class historyable : public npcname {
protected:
	short unsigned	classid;
	variant			values[5];
	class string;
public:
	void			act(stringbuilder& sb, const char* format, ...) const { actv(sb, format, xva_start(format)); }
	void			actv(stringbuilder& sb, const char* format, const char* format_param, bool add_space = true) const;
	void			actn(stringbuilder& sb, const char* format, const char* format_param) const;
	void			background() const;
	void			chooseclass();
	void			choosehistory();
	void			clear();
	const classi&	getclass() const { return bsdata<classi>::elements[classid]; }
};
struct speciali {
	const char*		id;
};
struct treasurei {
	const char*		id;
	char			abilities[6]; // Bonus to abilities
	short unsigned	index;
	flaga			tags, conditions; // Special tags
	variant			reduce, reduce_count; // reduce this
	variant			gain, gain_count;
	bool			ismagic() const { return szstart(id, "Magic"); }
	bool			isstory() const { return szstart(id, "Story"); }
	static void		sfgetinfo(const void* object, stringbuilder& sb);
};
class textable {
	char			data[32];
public:
	constexpr textable() : data{} {}
	void operator=(const char* v) { stringbuilder sb(data); sb.add(v); }
	operator const char*() const { return data; }
};
class pirate : public historyable {
	char			abilities[Infamy + 1];
	variant			treasures[4];
	void			afterchange(ability_s v);
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
	const treasurei* choosetreasure(const char* title, bool allow_stopbury) const;
	void			clear();
	void			clearactions();
	bool			confirm(ability_s v, int delta) const;
	void			information(const char* format, ...);
	void			gaintreasure(int count = 1);
	void			gaintreasure(const treasurei* p);
	void			generate();
	int				get(ability_s v) const { return abilities[v]; }
	static const char* getavatarst(const void* object);
	int				getbonus(ability_s v) const;
	int				getmaximum(ability_s v) const;
	int				getnextstar(int value) const;
	bool			match(variant v) const;
	static void		painttreasure();
	void			remove(variant v);
	void			raiseskills(int count);
	void			roll(special_s type);
	static void		sfgetproperty(const void* object, variant v, stringbuilder& sb);
	void			set(ability_s v, int i);
	void			sortactions();
	void			tradefriend();
	void			warning(const char* format, ...);
};
struct counter {
	short			current, maximum;
};
class shiplog {
	short unsigned	goal_id;
	bool			reach_location;
	taga			tags;
public:
	void			checkgoal();
	void			clear();
	static const char* getentryname(int v);
	const goali*	getgoal() const { return goal_id == 0xFFFF ? 0 : bsdata<goali>::elements + goal_id; }
	static void		listofgoals();
	static void		listofrecords();
	bool			istag(int v) const;
	void			setgoal(const goali* v) { goal_id = bsdata<goali>::source.indexof(v); }
	void			settag(int v);
	void			removetag(short unsigned v) { tags.remove(v); }
};
class gamei : public pirate, public oceani, public cannoneer, public shiplog {
	char			scenario[32];
	flagable<2>		locked;
	npcname			pirates[3];
	adat<indext, 512> treasures;
	adat<indext, 64> tiles;
public:
	static void		apply(variant v);
	void			createtiles();
	void			createtreasure();
	void			clear();
	static void		choosecounter();
	static void		choosehistory();
	void			chartacourse(int v);
	void			fullthrottle(int level);
	static int		getpage();
	static void		generate();
	bool			islocked(int i) const { return locked.is(i); }
	void			lock(int i) { locked.set(i); }
	static void		listofcounters();
	static void		listofgoals();
	const treasurei* picktreasure();
	indext			picktile();
	static void		playsail();
	static void		script(int page);
	static void		showseamap();
	void			setgoal(const goali* v) { }
	static bool		sfapply(variant v, bool run) { if(run) apply(v); return true; }
	static void		sfgetinfo(const void* object, stringbuilder& sb);
	static void		sfgetstatus(const void* object, stringbuilder& sb);
	static void		sfgetproperty(const void* object, variant v, stringbuilder& sb);
	void			unlockall() { locked.clear(); }
	void			unlock(int i) { locked.remove(i); }
};
extern gamei		game;
extern int			last_result, last_roll, last_bonus;
extern int			last_choose;
extern ability_s	last_ability;
int					rollv(int bonus);
VKIND(gender_s, Gender)
VKIND(special_s, Special)
VKIND(treasurei, Card)
VKIND(groupvaluei, Value)