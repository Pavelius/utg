#include "charname.h"
#include "gender.h"
#include "group.h"
#include "message.h"
#include "quest.h"
#include "utg.h"

#pragma once

template<typename T> struct bsmeta;

enum ability_s : unsigned char {
	Exploration, Brawl, Hunting, Aim, Swagger, Navigation,
	Reroll, Misfortune, Treasure,
	Experience, Stars, Level, Infamy,
};
enum shipstat_s : unsigned char {
	Crew, Discontent, Supply, Hull, Threat,
};
enum special_s : unsigned char {
	Name, Nickname, NicknameEnd,
	Change, Choose, Page, Roll,
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
	Ability, Action, Card, Case, Class, Gender, Group, Location, Menu, Pirate, Ship, Special, Tag, Value, Widget
};

typedef flagable<1> itemufa;
typedef char shipstata[Threat + 1];
typedef short unsigned indext;

struct abilityi {
	const char*		id;
};
struct actioni {
	const char*		id;
	variants		script;
	char			result[6];
	variants		outcome1, outcome2, outcome3, outcome4, outcome5, outcome6;
	variants		getoutcome(int v) const;
	int				getstage(int v) const;
};
struct casei {
	variant			type;
	variants		outcome;
	const char*		name;
};
struct locationi {
	const char*		id;
	variant			actions[7];
};
struct shipstati {
	const char*		id;
};
struct shipi {
	shipstata		stats;
	indext			index;
	int				get(shipstat_s v) const { return stats[v]; }
	indext			getposition() const { return index; }
	void			set(shipstat_s v, int i) { stats[v] = i; }
	void			setposition(indext v) { index = v; }
};
class oceani {
	short unsigned	data[7 * 6];
public:
	short unsigned	getlocation(indext i) const { return data[i]; }
	void			setlocation(indext i, int v) { data[i] = v; }
};
struct tagi {
	const char*		id;
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
};
class textable {
	char			data[32];
public:
	constexpr textable() : data{} {}
	void operator=(const char* v) { stringbuilder sb(data); sb.add(v); }
	operator const char*() const { return data; }
};
class pirate : public historyable {
	char			abilities[Level + 1];
	unsigned char	action, action_additional;
	unsigned short	treasures[4];
	void			afterchange(ability_s v);
	void			checkexperience(ability_s v);
	void			checkstars();
public:
	void			clear();
	void			clearactions();
	void			information(const char* format, ...);
	void			gaintreasure();
	void			gaintreasure(const treasurei* p);
	void			generate();
	int				get(ability_s v) const { return abilities[v]; }
	static const char* getavatarst(const void* object);
	int				getmaximum(ability_s v) const;
	int				getnextstar(int value) const;
	static void		getpropertyst(const void* object, variant v, stringbuilder& sb);
	void			roll();
	void			set(ability_s v, int i);
	void			setaction(int i);
	bool			isuse(int v) const;
	bool			match(variant v) const;
};
struct piratea : adat<pirate*, 6> {
	piratea();
	piratea(int action);
};
class gamei : public shipi, public oceani {
	char			scenario[32];
	unsigned char	location;
	adat<indext, 512> treasures;
public:
	adat<unsigned char, 8> pirates;
	static void		apply(variant v);
	static void		apply(const variants& source);
	void			createtreasure();
	void			clear();
	static void		choosehistory();
	pirate*			choosepirate(const char* title, const historyable* exclude) const;
	pirate*			getpirate(int order) const;
	static void		getpropertyst(const void* object, variant v, stringbuilder& sb);
	void			fillpirates();
	static void		play();
	const treasurei* picktreasure();
	static void		generate();
	locationi&		getlocation();
	static bool		match(variant v);
};
extern gamei		game;
extern int			last_result, last_roll, last_bonus;
extern ability_s	last_ability;
extern actioni*		last_action;
extern pirate*		last_pirate;
int					rollv(int bonus);
VKIND(actioni, Action)
VKIND(gender_s, Gender)
VKIND(pirate, Pirate)
VKIND(special_s, Special)
VKIND(groupvaluei, Value)