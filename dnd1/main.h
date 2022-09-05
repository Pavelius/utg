#include "avatarable.h"
#include "dice.h"
#include "chooseoption.h"
#include "gender.h"
#include "generator.h"
#include "list.h"
#include "nameable.h"
#include "utg.h"

#pragma once

enum ability_s : unsigned char {
	Strenght, Dexterity, Constitution, Intellect, Wisdow, Charisma,
	ToHit, Damage, MeleeToHit, MeleeDamage, RangedToHit, RangedDamage,
	AC, Level,
	HP, HPMax,
	Speed,
	SaveDeath, SaveWands, SaveParalize, SaveBreathWeapon, SaveSpells, SavePoison,
};
enum feat_s : unsigned char {
	EnergyDrain, Paralysis, PetrifyingGaze, PoisonImmunity, StrenghtDrain,
	SunSensitive,
	Undead, Summoned, Player, Enemy,
};
enum class_s : unsigned char {
	Monster,
	Cleric, Dwarf, Elf, Fighter, Halfling, Theif, Wizard,
};
enum duration_s : unsigned char {
	Instant,
	Round,
	Turn, Turn2, Turn3, Turn2d6, Turn4d4,
	Hour, Hour2,
	Concentration, Permanent,
};
enum range_s : unsigned char {
	Caster, CasterOrCreatureTouched, EnemyCreatureTouched,
	Range5, Range10, Range30, Range60, Range120, Range150, Range240,
};
enum reaction_s : unsigned char {
	Hostile, Unfriendly, Neutral, Indifferent, Friendly
};
enum spell_s : unsigned char {
	CureLightWound, DetectEvil, DetectMagic, Light, ProtectionFromEvil, PurifyFoodAndWater, RemoveFear, ResistCold,
	CharmPerson, FloatingDisc, HoldPortal, MagicMissile, ReadLanguages, ReadMagic, Shield, Sleep, Ventriloquism
};
enum wear_s : unsigned char {
	Backpack, BackpackLast = Backpack + 15,
	MeleeWeapon, MeleeWeaponOffhand, RangedWeapon, ThrownWeapon, Ammunition,
	Head, Torso, Legs, Gloves, FingerRight, FingerLeft, Elbows,
};
enum itemf_s : unsigned char {
	Blunt, Large, Martial, TwoHanded, WearLeather, WearIron, WearShield,
	Slow, Countable,
};
inline int d100() { return rand() % 100; }
inline int d6() { return 1 + rand() % 6; }
typedef flagable<8> spellf;
struct abilityi : nameable {
};
struct featable : flagable<4> {};
struct rangei : nameable {
};
struct bonusi {
	variant			owner;
	spell_s			effect;
	unsigned		rounds;
	void			clear() { memset(this, 0, sizeof(*this)); }
};
struct poweri {
	const char*		id;
};
struct damagei : nameable {
	feat_s			immunity;
};
struct itemi : nameable {
	struct weaponi {
		dice		damage;
		short 		ammunition;
	};
	struct armori {
		char		ac;
	};
	int				cost, weight;
	armori			armor;
	weaponi			weapon;
	wear_s			wear;
	flagable<4>		flags;
	bool			is(itemf_s v) const { return flags.is(v); }
};
struct item {
	unsigned char	type, subtype;
	union {
		unsigned short count;
		struct {
			unsigned char identified : 1;
			unsigned char charge : 5;
			unsigned char count_nocountable;
		};
	};
	constexpr item() : type(0), subtype(0), count(0) {}
	constexpr item(unsigned char t) : type(t), subtype(0), count(0) {}
	explicit operator bool() const { return type != 0; }
	void			add(item& v);
	void			addname(stringbuilder& sb) const;
	bool			canequip(wear_s v) const;
	void			clear() { memset(this, 0, sizeof(*this)); }
	void			create(const char* id, int count = 1);
	const itemi&	geti() const { return bsdata<itemi>::elements[type]; }
	int				getcount() const;
	dice			getdamage() const;
	bool			iscountable() const { return geti().is(Countable); }
	void			setcount(int v);
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
	char			abilities[SavePoison + 1];
	void			add(ability_s i, int v) { abilities[i] += v; }
	void			applybest(ability_s v);
	void			applyminimal(class_s v);
	void			equipmentbonus(const item& it);
	int				get(ability_s i) const { return abilities[i]; }
	ability_s		getbestability() const;
	void			rollability();
};
struct classi : nameable {
	ability_s		prime;
	char			minimal[6];
	int				tohit, hd;
};
struct equipmenti {
	class_s			type;
	unsigned char	equipment;
};
struct durationi : nameable {
	short			from, to;
};
struct feati : nameable {
};
struct itemfi : nameable {
};
struct spelli : nameable {
	char			level[3];
	duration_s		duration;
	range_s			range;
	dice			effect;
	//damage_s		damage;
};
struct spellable {
	unsigned char	spells[Ventriloquism + 1];
};
struct treasure : adat<item> {
	void			add(item it);
	void			generate(char symbol);
	void			take();
};
struct enchantmenti {
	char			level;
	char			magic;
	const char*		id;
	variant			special;
};
struct weari {
	const char*		id;
};
struct wearable {
	item			wears[Elbows + 1];
	void			additem(item& v);
	void			equip(item& v);
	bool			isitem(const void* pv) const;
};
struct creature : actable, spellable, statable, avatarable, wearable {
	class_s			type;
	statable		basic;
	featable		feats;
	spellf			active_spells;
	unsigned char	enemy_index;
	char			initiative;
	unsigned		experience;
	bool			apply(spell_s, bool run);
	bool			attack(ability_s attack, int ac, int bonus) const;
	void			choose(const slice<chooseoption>& options);
	void			clear();
	void			create(class_s type, gender_s gender);
	void			enchant(spell_s, unsigned rounds);
	void			damage(int value);
	void			dispell(spell_s effect);
	void			finish();
	void			generate();
	int				getbonus(ability_s v) const;
	int				getbonush(ability_s v) const;
	dice			getdamage(wear_s v) const;
	creature*		getenemy() const { return enemy_index == 0xFF ? 0 : bsdata<creature>::elements + enemy_index; }
	feat_s			getenemyfeat() const;
	int				gethit() const;
	static void		getproperty(const void* object, variant id, stringbuilder& sb);
	void			getstatus(stringbuilder& sb) const;
	static void		getstatus(const void* object, stringbuilder& sb);
	void			heal(int value) {}
	bool			is(spell_s v) const { return active_spells.is(v); }
	bool			is(feat_s v) const { return feats.is(v); }
	bool			isactive(spell_s v) const;
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
};
struct creaturea : adat<creature*, 32> {
	creature*		choose(const char* title) const;
	void			match(feat_s v, bool keep);
	void			matchenemy(bool keep);
	void			matchready(bool keep);
	void			select();
};
struct monsteri : nameable, statable {
	featable		feats;
};
struct timeable {
	unsigned		start_year;
	unsigned		rounds;
};
struct scene {
	creaturea		creatures;
	creature*		choosetarget() const;
	void			rollinitiative();
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
struct ongoing {
	spell_s			id;
	creature*		owner;
	unsigned		duration;
	static ongoing*	find(spell_s id, const creature* p);
};
void				combat_mode();
