#include "stringact.h"
#include "hero.h"
#include "unit.h"
#include "tactic.h"

#pragma once

struct army : adat<const uniti*, 24> {
	playeri*		player;
	provincei*		province;
	heroi*			hero;
	int				armor, strenght;
	const tactici*	tactic;
	costac			spoils;
	void			act(stringbuilder& sb, const char* format, ...) const;
	void			addprompt(stringbuilder& sb) const;
	void			addtotal(stringbuilder& sb, cost_s v) const;
	void			addunits(stringbuilder& sb, bool use_distinct = true) const;
	void			casualty(const army& list, army& spoils);
	void			clear() { memset(this, 0, sizeof(*this)); }
	void			damage(army& result, int value);
	bool			isattacker() const { return province->player != player; }
	int				get(cost_s v) const;
	int				get(cost_s v, const army* opponent, cost_s mv) const;
	int				geteffect(cost_s v) const;
	const char*		getname() const;
	const char*		getnameof() const;
	int				getunitcount(const tactici* v) const;
	void			fill(tactica& source);
	void			normalize();
	void			match(cost_s v, bool keep);
	void			select(const provincei* province);
	void			select(const provincei* province, const playeri* player);
	void			setcasualty(const army& source);
	void			sort();
	void			randomtactic();
};
