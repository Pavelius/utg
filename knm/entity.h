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
	void			focusing() const;
	int				get(ability_s v) const;
	playeri*		getplayer() const { return player; }
	provincei*		getprovince() const;
	uniti*			getunit() const;
	bool			is(const entity* v) const;
	bool			is(tag_s v) const { return taga::is(v); }
	void			set(const entity* v);
	void			set(tag_s v) { taga::set(v); }
};