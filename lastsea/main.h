#include "avatarable.h"
#include "charname.h"
#include "gender.h"
#include "group.h"
#include "message.h"
#include "utg.h"

#pragma once

template<typename T> struct bsmeta;

enum ability_s : unsigned char {
	Exploration, Brawl, Hunting, Aim, Swagger, Navigation,
	Experience, Stars, Level, Infamy,
};
enum special_s : unsigned char {
	Name, Nickname, NicknameEnd
};
enum variant_s : unsigned char {
	NoVariant,
	Ability, Class, Gender, Group, Pirate, Special, Value
};

typedef flagable<1> itemufa;
typedef char abilitya[Navigation + 1];

struct abilityi {
	const char*		id;
};
struct actioni {
	const char*		id;
};
struct statable {
	abilitya		abilities;
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
};
class historyable : public npcname {
protected:
	short unsigned	classid;
	variant			values[5];
	class string;
public:
	void			act(stringbuilder& sb, const char* format, ...) const { actv(sb, format, xva_start(format)); }
	void			actv(stringbuilder& sb, const char* format, const char* format_param) const;
	void			background() const;
	void			chooseclass();
	void			choosehistory();
	void			clear();
	const classi&	getclass() const { return bsdata<classi>::elements[classid]; }
};
struct speciali {
	const char*		id;
};
class pirate : public historyable, public avatarable, public statable {
public:
	void			clear();
	void			generate();
	int				get(ability_s v) const { return abilities[v]; }
	static const char* getavatarst(const void* object);
	bool			match(variant v) const;
};
class gamei {
	char			bolster;
public:
	static void		choosehistory();
	pirate*			choosepirate(const char* title, const historyable* exclude) const;
	static void		generate();
	int				getbolster() const { return bolster; }
	static bool		match(variant v);
	void			usebolster() { bolster--; }
};
extern gamei		game;
extern int			last_result, last_roll, last_bonus;
int					rollv(int bonus);
VKIND(gender_s, Gender)
VKIND(pirate, Pirate)
VKIND(special_s, Special)
VKIND(groupvaluei, Value)