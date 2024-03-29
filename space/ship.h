#include "action.h"
#include "moveable.h"
#include "module.h"

#pragma once

enum shipclass_s : unsigned char {
	Fighter, Fregate, Destroyer, Cruiser, Battleship, Carrier,
};

struct planeti;
struct shipclassi : nameable {
	short			size, hull, speed, armor, shield;
};
struct shipi : nameable {
	shipclass_s		kind;
	variants		elements;
	const shipclassi& geti() const;
};
struct ship : moveable, modulea {
	unsigned short	type, system, homeworld;
	short			hull, shield;
	int				credits;
	actionstate_s	state;
	modulea			basic;
	bool			cansee(const ship& v) const;
	const shipi&	geti() const;
	void			getinfo(stringbuilder& sb) const;
	int				getmaximum(module_s v) const;
	planeti*		gethomeworld() const;
	planeti*		getplanet() const;
	int				getpixelsize() const;
	int				getspeed() const { return modules[Engine]; }
	void			move(point position);
	void			paint() const;
	long			rangeto(const ship& v) const;
	void			recover();
	void			update();
	void			update_correction();
};

void play_player_turn();

extern ship* last_ship;
extern ship* player;
