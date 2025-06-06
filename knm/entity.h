#include "nameable.h"
#include "taga.h"

#pragma once

enum abilityn : unsigned char;
enum tag_s : unsigned char;

struct cardi;
struct playeri;
struct provincei;
struct uniti;

struct entity : nameable, taga {
	playeri*		player;
	entity*			location; // Can be province, board area, deck, something else
	void			clear();
	void			focusing() const;
	int				get(abilityn v) const;
	int				getbonus(abilityn v) const;
	cardi*			getcomponent() const;
	const char*		getname() const;
	playeri*		getplayer() const;
	provincei*		getprovince() const;
	uniti*			getunit() const;
	bool			is(const entity* v) const;
	bool			is(tag_s v) const { return taga::is(v); }
	bool			ishomeland() const;
	void			pick(playeri* v);
	void			set(const entity* v);
	void			set(tag_s v) { taga::set(v); }
	void			remove(const entity* v);
	void			remove(tag_s v) { taga::remove(v); }
};

void* get_province(const void* object);
void* get_unit(const void* object);