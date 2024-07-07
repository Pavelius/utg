#include "actable.h"
#include "avatarable.h"
#include "class.h"
#include "consumable.h"
#include "race.h"
#include "statable.h"
#include "wearable.h"

#pragma once

struct genderi;

struct creature : actable, statable, consumablea, wearable, avatarable {
	class_s		kind;
	featable	feats;
	statable	basic;
	short unsigned ancestry;
	short unsigned distance;
	short unsigned enemy, leader;
	short unsigned location;
	char		npc_index;
	void		clear();
	void		damage(int value);
	const char*	getkindname() const;
	dice		getdamage(ability_s ability) const;
	creature*	getenemy() const;
	const char*	getracename() const;
	bool		is(feat_s v) const { return feats.is(v); }
	bool		is(state_s v) const { return states.is(v); }
	bool		isallow(const item& v) const;
	bool		isenemy(const creature& v) const { return v.is(Enemy) != is(Enemy); }
	bool		ismelee() const;
	bool		isfollower() const { return leader != 0xFFFF; }
	bool		isplayer() const { return true; }
	void		kill();
	void		update();
};
extern creature* player;
extern creature* opponent;
extern genderi* last_gender;

void add_creature(const char* id, gender_s gender, class_s kind);
void add_creature(const char* id);
void add_creature(const struct monsteri* pm);
void add_npc_creature();

creature* getowner(const item& it);