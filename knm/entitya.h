#include "collection.h"
#include "entity.h"
#include "vector.h"

#pragma once

struct decki;
struct provincei;

struct entitya : collection<entity> {
	provincei*	chooseprovince() const;
	int			getarmy() const;
	int			gettotal(const entity* unit) const;
	int			gettotal(ability_s v) const;
	int			getplayercount() const;
	entity*		pick() { return (entity*)collectiona::pick(); }
	void		select(const entity* location);
	void		select(const entity* location, const entity* player);
	void		select(const entity* deck, const entity* location, const entity* player);
	void		sortcards();
	void		sortunits();
};
struct entityv : vector<entity*> {
	entityv() : vector<entity*>() {}
	entityv(const slice<entity*>& source) : vector<entity*>(source) {}
};
extern entitya querry;
