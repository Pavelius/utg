#include "actable.h"
#include "alignment.h"
#include "attackable.h"
#include "avatarable.h"
#include "monster.h"
#include "reaction.h"
#include "spell.h"
#include "statable.h"
#include "wearable.h"

#pragma once

struct creature : actable, spellable, statable, avatarable, wearable {
	unsigned short	leader, tight;
	alignmentn		alignment;
	reactionn		reaction;
	statable		basic;
	spellable		prepared;
	spellf			active_spells, known_spells;
	char			initiative;
	unsigned		experience;
	raceable		languages;
	void			clear();
	bool			cast(spelln spell, bool run);
	void			damage(int value);
	void			drink(spelln effect);
	void			equip(item& v);
	void			generate();
	int				get(abilityn v) const { return statable::get(v); }
	int				get(spelln v) const { return spells[v]; }
	int				getaward() const;
	creature*		getcontact() const;
	interval		getdamage(wear_s v) const;
	creature*		getleader() const;
	int				getlevel(spelln spell) const;
	static void		getproperty(const void* object, variant id, stringbuilder& sb);
	int				getspells(int level) const;
	void			getstatus(stringbuilder& sb) const;
	static void		getstatus(const void* object, stringbuilder& sb);
	void			heal(int value);
	bool			is(spelln v) const { return active_spells.is(v); }
	bool			is(feat_s v) const { return feats.is(v); }
	bool			isallow(const item& it) const;
	bool			isalive() const { return get(HP) > 0; }
	bool			isally() const;
	bool			ischaracter() const;
	bool			isenemy() const;
	bool			isknown(spelln v) const { return known_spells.is(v); }
	bool			isplayer() const;
	bool			isready() const;
	bool			iswounded() const;
	void			kill();
	void			levelup();
	void			raiselevel();
	void			remove();
	void			rest();
	bool			save(spelln id, int& count) const;
	void			set(feat_s v) { feats.set(v); }
	void			setleader(const creature* pv);
	void			setcontact(const creature* pv);
	void			update();
	void			update_finish();
	void			use(item& it);
	void			use(spelln spell);
};
extern creature *player, *opponent, *caster;
struct creaturea : adat<creature*, 32> {
	typedef bool (creature::*fnproc)() const;
	creature*		choose(const char* title, bool random = false) const;
	void			match(feat_s v, bool keep);
	void			match(fnproc proc, bool keep);
	void			matchally(bool keep);
	void			matchenemy(bool keep);
	void			matchready(bool keep) { match(&creature::isready, keep); }
	void			matchyou(bool keep) { match(&creature::isplayer, keep); }
	creature*		random() const;
	void			select();
	void			set(feat_s v);
};
extern creaturea creatures, targets;

void add_creature(const classi* pi, gendern gender, int level = 1);
void add_creature(const monsteri* pi);
void finish_creature();

const char* random_avatar(const classi* pi, gendern gender);

int get_average(abilityn v);
int get_maximal(abilityn v);
int get_minimal(abilityn v);
int random_name(const classi* pi, gendern gender);
