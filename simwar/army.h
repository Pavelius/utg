#include "province.h"
#include "tactic.h"

#pragma once

struct army {
	playeri*		player;
	provincei*		province;
	int				strenght, units, casualty;
	const tactici*	tactic;
	costa			spoils;
	operator bool() const { return units != 0; }
	void			act(stringbuilder& sb, const char* format, ...) const;
	void			addprompt(stringbuilder& sb) const;
	void			addtotal(stringbuilder& sb, costn v) const;
	void			addunits(stringbuilder& sb, bool use_distinct = true) const;
	void			clear() { memset(this, 0, sizeof(*this)); }
	void			damage(army& result, int value);
	bool			isattacker() const { return province->player != player; }
	int				get(costn v) const;
	int				get(costn v, const army* opponent, costn mv) const;
	int				geteffect(costn v) const;
	const char*		getname() const;
	const char*		getnameof() const;
	int				getunitcount(const tactici* v) const;
	void			normalize();
	void			match(costn v, bool keep);
	void			select(const provincei* province);
	void			select(const provincei* province, const playeri* player);
	void			setcasualty(const army& source);
	void			sort();
};
