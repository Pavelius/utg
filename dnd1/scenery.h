#pragma once

#include "nameable.h"
#include "parentable.h"
#include "spell.h"
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
	bool		apply(spell_s id, int level, bool run);
	void		enter();
	bool		haveitems() const;
	void		update();
};
extern scenery* scene;
