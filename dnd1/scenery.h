#pragma once

#include "nameable.h"
#include "parentable.h"
#include "typeable.h"

enum scenestat_s : unsigned char {
	Temperature, Rainess, Sun,
};
struct scenestata {
	char		abilities[Sun + 1];
	int			get(scenestat_s v) const { return abilities[v]; }
	bool		ishight(scenestat_s v) const { return get(v) >= 3; }
};
struct sceneryi : nameable {
};
struct scenery : typeable<sceneryi>, parentable<scenery>, scenestata {
	void		enter();
	bool		haveitems() const;
};
extern scenery* scene;
