#include "class.h"
#include "draw_utg.h"
#include "avatarable.h"
#include "dice.h"
#include "chooseoption.h"
#include "feat.h"
#include "gender.h"
#include "generator.h"
#include "list.h"
#include "nameable.h"
#include "quest.h"
#include "reaction.h"
#include "speech.h"
#include "spell.h"
#include "wearable.h"

#pragma once

inline int d6() { return 1 + rand() % 6; }
struct itema : adat<item*> {
	void			select(struct creature& source);
	void			match(wear_s wear, bool keep);
};
struct damagei : nameable {
	feat_s			immunity;
};
struct spella : adat<spell_s> {
	void			known(const struct creature& source);
	void			prepared(const struct creature& source);
};
struct actable {
	const char*		name;
	gender_s		gender;
	void			act(const char* format, ...) const { actv(*answers::console, format, xva_start(format), ' '); }
	void			actn(const char* format, ...) const { actv(*answers::console, format, xva_start(format), '\n'); }
	void			actv(stringbuilder& sb, const char* format, const char* format_param, char separator) const;
	const char*		getname() const { return name; }
};
struct statable {
	char			abilities[IllusionCopies + 1];
	void			add(ability_s i, int v) { abilities[i] += v; }
	void			applybest(ability_s v);
	void			applyminimal(class_s v);
	void			equipmentbonus(const item& it);
	int				get(ability_s i) const { return abilities[i]; }
	static int		getattackbonus(int type, int level);
	ability_s		getbestability() const;
	int				getbonus(ability_s v) const;
	int				getbonush(ability_s v) const;
	static unsigned	getexperience(class_s type, int level);
	char			getsave(class_s type, ability_s save, int level);
	void			rollability();
};
struct equipmenti {
	class_s			type;
	unsigned char	equipment;
};
struct creature : actable, spellable, statable, avatarable, wearable {
	class_s			type;
	statable		basic;
	featable		feats;
	spellf			active_spells, known_spells;
	unsigned char	enemy_index;
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
	creature*		getenemy() const { return enemy_index == 0xFF ? 0 : bsdata<creature>::elements + enemy_index; }
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
	void			setenemy(const creature* v);
	void			update();
	void			update_equipment();
	void			update_finish();
	void			update_spells();
	void			update_start();
	void			use(item& it);
};
struct creaturea : adat<creature*, 32> {
	creature*		choose(const char* title, bool random = false) const;
	void			match(feat_s v, bool keep);
	void			matchenemy(bool keep);
	void			matchready(bool keep);
	void			select();
};
struct monsteri : nameable, statable {
	featable		feats;
	dice			dungeon, wilderness;
};
struct timeable {
	unsigned		start_year;
	unsigned		rounds;
};
struct scene {
	creaturea		creatures;
	creature*		choosetarget() const;
	void			encounter(const monsteri* p);
	void			rollinitiative();
};
struct roomi {
	const char*		id;
	const char*		enter;
	const char*		look;
};
struct gamei : timeable, scene {
	reaction_s		reaction;
	bool			party_surprised, monster_surprised;
	void			initialize();
	static void		mainmenu();
	void			rollreaction(int bonus);
	void			rollsurprise();
};
extern creature* player;
extern creaturea targets;
extern gamei game;
void				combat_mode();
