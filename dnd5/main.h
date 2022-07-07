#include "flagable.h"
#include "dice.h"
#include "script.h"
#include "tag.h"

#pragma once

enum modifier_s : unsigned char {
	NoModififer,
	Proficient,
	Immunity, Resist, Vulnerable,
};
enum tag_s : unsigned char {
	Darkvision, Stonecunning, Toughness
};

struct nameable {
	const char*		id;
};
struct abilityi : nameable {
};
struct classi : nameable {
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
struct advancei : nameable {
	variant			base;
	char			level;
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
	void			load(statable& e) { *this = e; }
};
struct moveable : nameable, statable {
	statable		basic;
	const racei*	race;
	void			apply(variant v);
	void			apply(const variants& v);
	void			update();
};