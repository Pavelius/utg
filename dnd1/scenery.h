#pragma once

#include "nameable.h"
#include "group.h"
#include "feat.h"
#include "parentable.h"
#include "spell.h"
#include "typeable.h"
#include "variant.h"

struct scenei : nameable {
	variant		actions;
	variants	elements;
	group_s		encounter;
	char		encounter_chance;
};
struct scenery : typeable<scenei>, parentable<scenery> {
	featable	flags;
	bool		apply(spell_s id, int level, bool run);
	void		clear();
	void		enter();
	bool		haveitems() const;
	bool		is(feat_s v) const { return flags.is(v); }
	bool		is(spell_s v) const;
	void		set(feat_s v) { flags.set(v); }
	void		remove(feat_s v) { flags.remove(v); }
	void		update();
};
extern scenery* scene;

void add_scene(const char* id);

bool have_feats(feat_s v, bool keep = true);
