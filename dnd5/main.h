#include "answers.h"
#include "flagable.h"
#include "gender.h"
#include "dice.h"
#include "menu.h"
#include "script.h"
#include "tag.h"

#pragma once

enum modifier_s : unsigned char {
	Proficient,
	Immunity, Resist, Vulnerable,
};
enum tag_s : unsigned char {
	Darkvision, Stonecunning, Toughness,
	ImmuneToSleepMagic, HideInEnviroment,
	RitualCasting,
};

struct nameable {
	const char*		id;
};
struct abilityi : nameable {
};
struct alignmenti : nameable {
};
struct classi : nameable {
	char			hd;
};
struct damagei : nameable {
};
struct itemi : nameable {
	struct weaponi {
		dice		damage;
	};
	weaponi			weapon;
};
struct listi : nameable {
	variants		elements;
};
struct languagei : nameable {
};
struct modifieri : nameable {
	static modifier_s last;
};
struct racei : nameable {
	racei*			parent;
};
struct skilli : nameable {
};
struct spelli : nameable {
	char			level;
};
struct advancei : nameable {
	char			level;
	variant			base;
	char			choose;
	variants		effect;
};
struct statable {
	char			abilitites[32];
	flagable<2>		saves;
	flagable<4>		skills;
	flagable<8>		items;
	flagable<2>		resist, immunity, vulnerable;
	flagable<4>		tags;
	bool			addstart(variant v, modifier_s modifier, bool run);
	void			load(statable& e) { *this = e; }
};
struct actable : nameable {
	const racei*	race;
	gender_s		gender;
};
struct moveable : actable, statable {
	statable		basic;
	unsigned char	alignment;
	char			classes[16];
	void			advance(variant base, int level);
	bool			addstart(variant v, modifier_s modifier, bool run);
	void			apply(const variants& v);
	void			apply(const advancei& v);
	void			update();
};
struct character : moveable {
	void			clear();
	void			generate();
};