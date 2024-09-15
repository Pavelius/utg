#include "province.h"

#pragma once

struct army {
	playeri*		player;
	provincei*		province;
	int				units, casualty;
	costa			result;
	operator bool() const { return units != 0; }
	void			act(stringbuilder& sb, const char* format, ...) const;
	void			actn(stringbuilder& sb, const char* format, ...) const;
	void			addprompt(stringbuilder& sb) const;
	void			addtotal(stringbuilder& sb, costn v) const;
	void			attackers();
	void			clear() { memset(this, 0, sizeof(*this)); }
	void			damage(int value);
	void			defenders();
	bool			isattacker() const { return province->player != player; }
	int				get(costn v) const;
	int				get(costn v, const army* opponent, costn mv) const;
	const char*		getname() const;
	const char*		getnameof() const;
	int				getstrenght() const { return get(Strenght) + units; }
};
