#include "province.h"
#include "tactic.h"

#pragma once

struct army {
	playeri*		player;
	provincei*		province;
	int				strenght, units, casualty;
	costa			spoils;
	operator bool() const { return units != 0; }
	void			act(stringbuilder& sb, const char* format, ...) const;
	void			addprompt(stringbuilder& sb) const;
	void			addtotal(stringbuilder& sb, costn v) const;
	void			attackers();
	void			clear() { memset(this, 0, sizeof(*this)); }
	void			damage(int value);
	void			defenders();
	bool			isattacker() const { return province->player != player; }
	int				get(costn v) const;
	int				get(costn v, const army* opponent, costn mv) const;
	int				geteffect(costn v) const;
	const char*		getname() const;
	const char*		getnameof() const;
};
