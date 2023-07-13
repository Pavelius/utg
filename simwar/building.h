#include "site.h"

#pragma once

struct buildingi : sitei {
	const buildingi*	upgrade;
	void				getinfo(stringbuilder& sb) const;
};
struct building {
	const buildingi*	type;
	provincei*			province;
	explicit operator bool() const { return type != 0; }
	void				clear();
	void				getinfo(stringbuilder& sb) const;
};
extern buildingi* lastbuilding;