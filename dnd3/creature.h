#include "consumable.h"
#include "enumerator.h"
#include "gender.h"
#include "resable.h"
#include "statable.h"
#include "variant.h"
#include "wearable.h"

#pragma once

struct monsteri;

struct creature : resable, statable, consumable, enumerable, wearable {
	statable	basic;
	void		clear();
	void		create();
	static void	create(const char* id);
	static void	create(const monsteri* pm);
	void		paint() const;
	void		update();
};
extern creature* player;
extern creature* party[8];
