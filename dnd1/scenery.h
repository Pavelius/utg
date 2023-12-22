#pragma once

#include "nameable.h"
#include "feat.h"
#include "parentable.h"
#include "spell.h"
#include "typeable.h"

enum scene_f : unsigned char {
	Outdoor,
};

struct scenefi : nameable {
};
struct sceneryi : nameable {
	flagable<4>	traits;
	featable	flags;
	bool		is(scene_f v) const { return traits.is(v); }
};
struct scenery : typeable<sceneryi>, parentable<scenery> {
	featable	flags;
	bool		apply(spell_s id, int level, bool run);
	void		clear();
	void		enter();
	bool		haveitems() const;
	bool		is(feat_s v) const { return flags.is(v); }
	bool		is(spell_s v) const;
	bool		is(scene_f v) const { return geti().is(v); }
	void		set(feat_s v) { flags.set(v); }
	void		remove(feat_s v) { flags.remove(v); }
	void		update();
};
extern scenery* scene;

void add_scene(const char* id);
