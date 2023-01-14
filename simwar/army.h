#include "stringact.h"
#include "hero.h"
#include "unit.h"
#include "tactic.h"

#pragma once

struct army : adat<const uniti*, 24> {
	playeri*		player;
	provincei*		province;
	heroi*			hero;
	const tactici*	tactic;
	void			act(stringbuilder& sb, const char* format, ...) const;
	void			addprompt(stringbuilder& sb) const;
	void			addtotal(stringbuilder& sb, const char* start, cost_s v) const;
	void			addunits(stringbuilder& sb) const;
	void			clear() { memset(this, 0, sizeof(*this)); }
	int				get(cost_s v) const;
	int				geteffect(cost_s v) const;
	const char*		getname() const;
	const char*		getnameof() const;
	int				getunitcount(const tactici* v) const;
	void			fill(tactica& source);
	void			match(cost_s v, bool keep);
	void			select(const provincei* province);
	void			select(const provincei* province, const playeri* player);
	void			randomtactic();
};
