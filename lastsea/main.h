#include "avatarable.h"
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
	Experience, Stars, Level, Infamy,
};
enum shipstat_s : unsigned char {
	Crew, Discontent, Supply, Hull, Threat,
};
enum special_s : unsigned char {
	Zero, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,
	Name, Nickname, NicknameEnd,
	Plus, Minus,
	Choose, Roll,
};
enum variant_s : unsigned char {
	NoVariant,
	Ability, Action, Class, Gender, Group, Location, Pirate, Special, Value
};

typedef flagable<1> itemufa;
typedef char abilitya[Navigation + 1];
typedef char shipstata[Threat + 1];
typedef short unsigned indext;

struct abilityi {
	const char*		id;
};
struct actioni {
	const char*		id;
	variants		script;
	char			result[6];
	void			play() const;
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
class textable {
	char			data[32];
public:
	constexpr textable() : data{} {}
	void operator=(const char* v) { stringbuilder sb(data); sb.add(v); }
	operator const char*() const { return data; }
};
class pirate : public historyable, public avatarable {
	abilitya		abilities;
	unsigned char	action, action_additional;
	void			afterchange(ability_s v);
	void			checkexperience(ability_s v);
	void			checkstars();
public:
	void			clear();
	void			clearactions();
	void			information(const char* format, ...);
	void			generate();
	int				get(ability_s v) const { return abilities[v]; }
	static const char* getavatarst(const void* object);
	int				getmaximum(ability_s v) const;
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
public:
	adat<unsigned char, 8> pirates;
	static void		apply(variant v);
	static void		apply(const variants& source);
	void			clear();
	static void		choosehistory();
	pirate*			choosepirate(const char* title, const historyable* exclude) const;
	pirate*			getpirate(int order) const;
	void			fillpirates();
	static void		play();
	static void		generate();
	locationi&		getlocation();
	static bool		match(variant v);
};
extern gamei		game;
extern int			last_result, last_roll, last_bonus;
extern ability_s	last_ability;
extern pirate*		last_pirate;
int					rollv(int bonus);
VKIND(actioni, Action)
VKIND(gender_s, Gender)
VKIND(pirate, Pirate)
VKIND(special_s, Special)
VKIND(groupvaluei, Value)