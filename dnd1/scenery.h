#pragma once

#include "nameable.h"
#include "group.h"
#include "feat.h"
#include "parentable.h"
#include "spell.h"
#include "typeable.h"
#include "variant.h"

struct scenei : nameable {
	variant		actions, script;
	variants	elements;
	group_s		encounter;
	char		encounter_chance;
};
struct scenery : typeable<scenei>, parentable<scenery> {
	featable	flags;
	char		count;
	void		clear();
	void		enter();
	const char* getid() const { return geti().getid(); }
	bool		haveitems() const;
	bool		is(featn v) const { return flags.is(v); }
	bool		is(spelln v) const;
	void		set(featn v) { flags.set(v); }
	void		remove(featn v) { flags.remove(v); }
	void		update();
};
extern scenery* scene;
extern int		encountered_count;

void add_scene(const char* id);

bool have_feats(featn v, bool keep = true);
