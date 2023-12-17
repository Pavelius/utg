#include "actable.h"
#include "attackable.h"
#include "avatarable.h"
#include "chooseoption.h"
#include "monster.h"
#include "spell.h"
#include "statable.h"
#include "wearable.h"

#pragma once

struct creature : actable, attackable, spellable, statable, avatarable, wearable {
	unsigned char	type;
	unsigned short	leader;
	statable		basic;
	spellf			active_spells, known_spells;
	char			initiative;
	unsigned		experience;
	raceable		languages;
	bool			apply(spell_s, int level, bool run);
	bool			attack(ability_s attack, int ac, int bonus);
	void			choose(const slice<chooseoption>& options, bool enemy_choose_first = false);
	void			clear();
	void			cast(spell_s spell);
	void			damage(int value);
	void			dispell(spell_s effect);
	void			drink(spell_s effect);
	void			enchant(spell_s, unsigned rounds);
	void			equip(item& v);
	void			generate();
	const classi&	geti() const;
	int				getaward() const;
	dice			getdamage(wear_s v) const;
	creature*		getleader() const;
	static void		getproperty(const void* object, variant id, stringbuilder& sb);
	void			getstatus(stringbuilder& sb) const;
	static void		getstatus(const void* object, stringbuilder& sb);
	void			heal(int value) {}
	bool			is(spell_s v) const { return active_spells.is(v); }
	bool			is(feat_s v) const { return feats.is(v); }
	bool			isallow(const item& it) const;
	bool			isknown(spell_s v) const { return known_spells.is(v); }
	bool			isplayer() const;
	bool			isready() const;
	void			levelup();
	void			raiselevel();
	void			remove();
	void			set(feat_s v) { feats.set(v); }
	void			setleader(const creature* pv);
	void			update();
	void			update_finish();
	void			use(item& it);
};
extern creature *player, *opponent;
struct creaturea : adat<creature*, 32> {
	typedef bool (creature::*fnproc)() const;
	creature*		choose(const char* title, bool random = false) const;
	void			match(feat_s v, bool keep);
	void			match(fnproc proc, bool keep);
	void			matchally(bool keep);
	void			matchenemy(bool keep);
	void			matchready(bool keep) { match(&creature::isready, keep); }
	void			matchyou(bool keep) { match(&creature::isplayer, keep); }
	void			select();
};
extern creaturea creatures, targets;

void add_creature(const classi* pi, gender_s gender);
void add_creature(const monsteri* pi);
void finish_creature();

const char* random_avatar(const classi* pi, gender_s gender);
const char* random_name(const classi* pi, gender_s gender);
