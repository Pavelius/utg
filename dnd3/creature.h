#include "consumable.h"
#include "resable.h"
#include "statable.h"
#include "wearable.h"

#pragma once

struct monsteri;

struct creature : resable, statable, consumable, wearable {
	statable	basic;
	void		clear();
	void		create();
	static void	create(const char* id);
	static void	create(const monsteri* pm);
	void		paint() const;
	void		update();
};
extern creature* player;
