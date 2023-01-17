#include "actable.h"
#include "avatarable.h"
#include "bsref.h"
#include "chooseoption.h"
#include "monster.h"
#include "spell.h"
#include "statable.h"
#include "wearable.h"

#pragma once

struct equipmenti {
	class_s			type;
	unsigned char	equipment;
};
struct creature : actable, spellable, statable, avatarable, wearable {
	class_s			type;
	statable		basic;
	featable		feats;
	spellf			active_spells, known_spells;
	creature*		enemy;
	char			initiative;
	unsigned		experience;
	bool			apply(spell_s, int level, bool run);
	bool			attack(ability_s attack, int ac, int bonus);
	void			choose(const slice<chooseoption>& options);
	void			clear();
	void			cast(spell_s spell);
	void			create(class_s type, gender_s gender);
	void			create(const struct monsteri& v);
	void			damage(int value);
	void			dispell(spell_s effect);
	void			drink(spell_s effect);
	void			enchant(spell_s, unsigned rounds);
	void			finish();
	void			generate();
	dice			getdamage(wear_s v) const;
	feat_s			getenemyfeat() const;
	static void		getproperty(const void* object, variant id, stringbuilder& sb);
	void			getstatus(stringbuilder& sb) const;
	static void		getstatus(const void* object, stringbuilder& sb);
	void			heal(int value) {}
	bool			is(spell_s v) const { return active_spells.is(v); }
	bool			is(feat_s v) const { return feats.is(v); }
	bool			isknown(spell_s v) const { return known_spells.is(v); }
	bool			isready() const;
	void			levelup();
	void			meleeattack();
	void			raiselevel();
	void			rangeattack(creature* enemy);
	static const char* randomname(class_s type, gender_s gender);
	static const char* randomavatar(class_s type, gender_s gender);
	void			set(feat_s v) { feats.set(v); }
	void			setenemy(creature* v);
	void			update();
	void			update_equipment();
	void			update_finish();
	void			update_spells();
	void			update_start();
	void			use(item& it);
};
extern creature* player;
struct creaturea : adat<creature*, 32> {
	creature*		choose(const char* title, bool random = false) const;
	void			match(feat_s v, bool keep);
	void			matchenemy(bool keep);
	void			matchready(bool keep);
	void			select();
};
extern creaturea creatures, targets;
