#pragma once

#include "nameable.h"
#include "feat.h"
#include "parentable.h"
#include "spell.h"
#include "typeable.h"

struct sceneryi : nameable {
};
struct scenery : typeable<sceneryi>, parentable<scenery> {
	featable	flags;
	bool		apply(spell_s id, int level, bool run);
	void		enter();
	bool		haveitems() const;
	bool		is(feat_s v) const { return flags.is(v); }
	bool		is(spell_s v) const;
	void		set(feat_s v) { flags.set(v); }
	void		remove(feat_s v) { flags.remove(v); }
	void		update();
};
extern scenery* scene;
