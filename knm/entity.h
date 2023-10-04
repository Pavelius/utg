#include "nameable.h"
#include "taga.h"

#pragma once

enum ability_s : unsigned char;
enum tag_s : unsigned char;

struct playeri;
struct provincei;
struct uniti;

struct entity : nameable, taga {
	playeri*		player;
	entity*			location;
	int				get(ability_s v) const;
	playeri*		getplayer() const { return player; }
	provincei*		getprovince() const;
	uniti*			getunit() const;
	bool			is(const playeri* v) const;
	void			set(const playeri* v);
};