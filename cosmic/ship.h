#include "module.h"
#include "typeable.h"
#include "variant.h"

#pragma once

struct shipclassi : nameable {
	variants		elements;
};
struct shipi : nameable, typeable<shipclassi> {
	variants		elements;
	const char*		getname() const { return nameable::getname(); }
};
struct ship : typeable<shipi>, modulea {
	explicit operator bool() const { return hull != 0; }
	short			hull, shield;
	int				credits;
	modulea			basic;
	bool			cansee(const ship& v) const;
	//void			getinfo(stringbuilder& sb) const;
	int				getspeed() const { return modules[Engine]; }
	int				getmaximum(modulen v) const;
	void			paint() const;
	void			update();
};

void play_player_turn();

extern ship* player;
extern ship* opponent;
