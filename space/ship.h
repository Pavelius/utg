#include "action.h"
#include "moveable.h"
#include "shipable.h"

#pragma once

enum shipclass_s : unsigned char {
	Fighter, Fregate, Destroyer, Cruiser, Battleship,
	Carrier,
};

struct planeti;
struct shipclassi : nameable {
	short			hull, speed, armor, shield;
};
struct shipi : modulei {
	shipclass_s		kind;
	char			size;
	variants		elements;
	const shipclassi& geti() const;
};
struct ship : moveable, shipable {
	unsigned short	type, system, homeworld;
	short			hull, shield;
	actionstate_s	state;
	shipable		basic;
	bool			cansee(const ship& v) const;
	const shipi&	geti() const;
	void			getinfo(stringbuilder& sb) const;
	planeti*		gethomeworld() const;
	planeti*		getplanet() const;
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
