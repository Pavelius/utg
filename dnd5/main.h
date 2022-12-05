#include "advance.h"
#include "answers.h"
#include "actable.h"
#include "flagable.h"
#include "gender.h"
#include "dice.h"
#include "menu.h"
#include "nameable.h"
#include "script.h"
#include "tag.h"

#pragma once

struct statable {
	char		abilitites[32];
	flagable<2>	saves;
	flagable<4>	skills;
	flagable<8>	items;
	flagable<2>	resist, immunity, vulnerable;
	flagable<4>	tags;
	bool		addstart(variant v, enum modifier_s modifier, bool run);
	void		load(statable& e) { *this = e; }
};
struct moveable : actable, statable {
	statable	basic;
	unsigned char	alignment;
	char		classes[16];
	void		advance(variant base, int level);
	bool		addstart(variant v, enum modifier_s modifier, bool run);
	void		apply(const variants& v);
	void		apply(const advancei& v);
	void		update();
};
struct character : moveable {
	void		clear();
	void		generate();
};