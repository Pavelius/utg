#include "actable.h"
#include "animal.h"
#include "bsdata.h"

#pragma once

class natureable : public actable {
	short unsigned	kind, name;
public:
	void			act(const char* format, ...) const { actv(sb, format, xva_start(format), '\n', getname(), getgender()); }
	const animali&	getanimal() const { return bsdata<animali>::elements[kind]; }
	const char*		getname() const;
	void			setname(short unsigned v) { name = v; }
};
