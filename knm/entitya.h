#include "collection.h"
#include "entity.h"

#pragma once

struct decki;
struct provincei;

struct entitya : collection<entity> {
	provincei*	chooseprovince() const;
	int			getarmy() const;
	int			gettotal(ability_s v) const;
	int			getplayercount() const;
	entity*		pick() { return (entity*)collectiona::pick(); }
	void		select(const entity* location);
	void		select(const entity* location, const entity* player);
	void		select(const entity* deck, const entity* location, const entity* player);
	void		sortunits();
};
struct entityv : vector<entity*> {
	entityv() : vector<entity*>() {}
	entityv(const collection<entity>& source) : vector<entity*>() { create(source.data, source.getcount()); }
};
extern entitya querry;
