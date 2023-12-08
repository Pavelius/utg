#pragma once

#include "collection.h"
#include "nameable.h"

struct areai : nameable {
};
struct area {
	unsigned short	type, parent;
	void			addlink(const area* to);
	const areai&	geti() const;
	area*			getparent() const;
	void			setparent(area* v);
};
extern collection<area> areas;