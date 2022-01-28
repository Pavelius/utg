#include "charname.h"
#include "counter.h"
#include "gender.h"
#include "group.h"
#include "message.h"
#include "pathfind.h"
#include "quest.h"
#include "utg.h"

#pragma once

template<typename T> struct bsmeta;

enum ability_s : unsigned char {
	Exploration, Brawl, Hunting, Aim, Swagger, Navigation,
	Reroll, Misfortune, Treasure,
	Crew, Discontent, Supply, Hull,
	Danger, DangerMaximum,
	Threat, Mission, Cabine,
	CounterA, CounterB, CounterC, CounterD,
	Stars, Level, Infamy,
	Gun1, Gun2, Gun3, Gun4,
	Tresure1, Tresure2, Tresure3, Tresure4,
};
enum special_s : unsigned char {
	Name, Nickname, NicknameEnd,
	Block, Choose, Roll, Bury, Scout, Steal, Skill, Scene,
	Tile000, Tile900, TileRock, AddTile, RemoveTile, SetShip,
	PaySupply, GunLoadOrHull, ReloadGun, UpgradeGun, AddGun, AddGunUnloaded,
	VisitManyTimes, VisitRequired, CheckDanger, PlayStars, Sail, LostGame, WinGame,
	Page000, Page100, Page200, Page300, Page400, Page500, Page600, Page700, Page800, Page900,
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
	Ability, Card, Class, Gender, Group, Menu, NavigationTile, Quest, Special, Tag, Value, Widget
};
enum ability_type_s : unsigned char {
	Positive, Negative,
};

typedef flagable<1> itemufa;
typedef short unsigned indext;

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
	void			createselections(const slice<indext>& selectable) const;
public:
	void			addpossiblecourse() const;
	indext			choose(const char* title) const;
	void			createobjects() const;
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
	void			showsplash();
	static indext	to(indext i, int direction);
};
struct tagi {
	const char*		id;
};
struct goali {
	const char*		id;
	counter			success, fail;
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
	indext			actions[6];
	variant			treasures[4];
	void			afterchange(ability_s v);
	void			checkexperience(ability_s v);
	void			chooseactions(int scene);
	void			playactions();
	void			playaction(int id);
	void			playchoose(int id);
	void			sortactions();
public:
	void			adventure(int page);
	void			addaction(indext v);
	void			afterapply();
	void			bury(int count);
	void			captaincabine();
	void			captainmission();
	const treasurei* choosetreasure(const char* title) const;
	void			clear();
	void			clearactions();
	bool			confirm(ability_s v, int delta) const;
	void			endscene(int scene);
	void			information(const char* format, ...);
	void			makethreat();
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
	void			playscene(int scene);
	void			remove(variant v);
	void			roll();
	static void		sfgetproperty(const void* object, variant v, stringbuilder& sb);
	void			set(ability_s v, int i);
	void			warning(const char* format, ...);
};
class cannoneer {
	unsigned char	guns[4];
	int				getgun(int level, bool loaded) const { return 1 + (level - 1) * 2 + (loaded ? 1 : 0); }
	int				getlevel(int v) const { return v ? 1 + (v - 1) / 2 : 0; }
	bool			isloaded(int v) const { return (v % 2) == 0; }
public:
	bool			addgun(int level, bool loaded, bool run);
	void			clearweapons() { memset(this, 0, sizeof(*this)); }
	int				getgunlevel(int index) const;
	bool			isgunloaded(int index) const;
	bool			reloadgun(int level, bool run);
};
class gamei : public pirate, public oceani, public cannoneer {
	char			scenario[32];
	flagable<2>		locked;
	adat<indext, 512> treasures;
	adat<indext, 64> tiles;
public:
	unsigned short	scene;
	static void		apply(variant v);
	static void		apply(const variants& source);
	void			createtiles();
	void			createtreasure();
	void			clear();
	static void		choosehistory();
	void			chartacourse(int v);
	bool			islocked(int i) const { return locked.is(i); }
	void			lock(int i) { locked.set(i); }
	static void		generate();
	const treasurei* picktreasure();
	indext			picktile();
	static void		playscene();
	static bool		sfapply(variant v, bool run) { if(run) apply(v); return true; }
	static void		sfgetinfo(const void* object, stringbuilder& sb);
	static void		sfgetstatus(const void* object, stringbuilder& sb);
	static void		sfgetproperty(const void* object, variant v, stringbuilder& sb);
	void			unlockall() { locked.clear(); }
	void			unlock(int i) { locked.remove(i); }
};
extern gamei		game;
extern int			last_result, last_roll, last_bonus;
extern int			last_choose, last_page, last_scene;
extern ability_s	last_ability;
int					rollv(int bonus);
VKIND(gender_s, Gender)
VKIND(special_s, Special)
VKIND(treasurei, Card)
VKIND(groupvaluei, Value)