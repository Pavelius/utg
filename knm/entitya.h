#include "collection.h"
#include "entity.h"

#pragma once

enum deck_s : unsigned char;

struct provincei;

struct entitya : collection<entity> {
	provincei*	chooseprovince() const;
	int			gettotal(ability_s v) const;
	int			getarmy() const;
	entity*		pick();
	void		select(const entity* object);
};
struct entityv : vector<entity*> {
	entityv() : vector<entity*>() {}
	entityv(const collection<entity>& source) : vector<entity*>() { create(source.data, source.getcount()); }
};
extern entitya querry;
