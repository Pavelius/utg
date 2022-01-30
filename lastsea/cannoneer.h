#include "crt.h"

#pragma once

class cannoneer {
	unsigned char	guns[4];
	int				getgun(int level, bool loaded) const { return 1 + (level - 1) * 2 + (loaded ? 1 : 0); }
	int				getlevel(int v) const { return v ? 1 + (v - 1) / 2 : 0; }
	bool			isloaded(int v) const { return (v % 2) == 0; }
public:
	bool			addgun(int level, bool loaded, bool run);
	void			clearweapons() { memset(this, 0, sizeof(*this)); }
	static int		getgunbonus(int level);
	int				getgunlevel(int index) const;
	bool			is(int level, bool loaded) const;
	bool			isgunloaded(int index) const;
	bool			reloadgun(int level, bool run);
	bool			unloadgun(int level, bool run);
};
