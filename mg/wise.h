#include "flagable.h"
#include "nameable.h"
#include "variant.h"

#pragma once

struct wisei : nameable {
	variant		subject;
};
typedef flagable<16> wisea;
class wiseable {
	wisea		wises;
public:
	bool		iswise(const wisei* p) const { return p && wises.is(getbsi(p)); }
	void		setwise(const wisei* p) { if(p) wises.set(getbsi(p)); }
};