#pragma once

#include "nameable.h"
#include "parentable.h"
#include "spell.h"
#include "typeable.h"

struct sceneryi : nameable {
};
struct scenery : typeable<sceneryi>, parentable<scenery> {
	bool		apply(spell_s id, int level, bool run);
	void		enter();
	bool		haveitems() const;
	void		update();
};
extern scenery* scene;
