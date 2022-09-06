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
	HPMax,
	Speed, Search,
	ClimbWalls, FindOrRemoveTraps, HearNoise, HideInShadows, MoveSilently, OpenLocks, PickPockets,
	Saves, SaveDeath, SaveWands, SaveParalize, SaveBreathWeapon, SaveSpells, SavePoison,
	HP, IllusionCopies,
};
enum feat_s : unsigned char {
	EnergyDrain, Paralysis, PetrifyingGaze, PoisonImmunity, StrenghtDrain,
	SunSensitive, Slow, NormalWeaponImmunity,
	Blunt, Martial, TwoHanded,
	WearLeather, WearIron, WearLarge, WearShield, Countable,
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
	Hour, Hour2, Hour8,
	Concentration, Permanent,
};
enum range_s : unsigned char {
	Caster, CasterOrCreatureTouched, EnemyCreatureTouched, AllCreatures,
	Range5, Range10, Range30, Range60, Range120, Range150, Range240,
};
enum reaction_s : unsigned char {
	Hostile, Unfriendly, Neutral, Indifferent, Friendly
};
enum spell_s : unsigned char {
	CureLightWound, DetectEvil, DetectMagic, Light, ProtectionFromEvil, PurifyFoodAndWater, RemoveFear, ResistCold,
	CharmPerson, FloatingDisc, HoldPortal, MagicMissile, ReadLanguages, ReadMagic, Shield, Sleep, Ventriloquism,
	ContinualLight, DetectInvisibility, ESP, Invisibility, Knock, Levitation, MirrorImages, PhantasmalForce, Web, WizardLock,
	LastSpell = WizardLock,
	ShrinkSize, GrowthSize, GaseousForm, DeathPoison,
};
enum wear_s : unsigned char {
	Backpack, Potion, BackpackLast = Backpack + 15,
	MeleeWeapon, MeleeWeaponOffhand, RangedWeapon, ThrownWeapon, Ammunition,
	Head, Torso, Legs, Gloves, FingerRight, FingerLeft, Elbows,
};
inline int d100() { return rand() % 100; }
inline int d6() { return 1 + rand() % 6; }
typedef flagable<8> spellf;
struct abilityi : nameable {
};
struct featable : flagable<4> {};
struct rangei : nameable {
};
struct ongoing {
	variant			owner;
	spell_s			effect;
	unsigned		rounds;
	void			clear() { memset(this, 0, sizeof(*this)); }
};
struct damagei : nameable {
	feat_s			immunity;
};
struct enchantmenti {
	const char*		id;
	variant			special;
	const char*		id_title;
};
struct itemi : nameable {
	struct weaponi {
		dice		damage;
		short 		ammunition;
	};
	struct armori {
		char		ac;
	};
	int				cost, weight, count;
	armori			armor;
	weaponi			weapon;
	wear_s			wear;
	featable		flags;
	const enchantmenti* enchantments;
	unsigned		enchantments_count;
	bool			is(feat_s v) const { return flags.is(v); }
};
struct item {
	unsigned char	type, subtype;
	union {
		unsigned short count;
		struct {
			unsigned char identified : 1;
			unsigned char broken : 1;
			unsigned char charge : 5;
			unsigned char count_nocountable;
		};
	};
	explicit operator bool() const { return type != 0; }
	void			add(item& v);
	void			addname(stringbuilder& sb) const;
	bool			canequip(wear_s v) const;
	void			clear() { memset(this, 0, sizeof(*this)); }
	void			create(const char* id, int count = 1);
	void			create(const itemi* pi, int count = 1);
	const itemi&	geti() const { return bsdata<itemi>::elements[type]; }
	int				getcost() const;
	int				getcount() const;
	dice			getdamage() const;
	const enchantmenti* getenchant() const;
	const char*		getname() const { return geti().getname(); }
	void			getstatus(stringbuilder& sb) const;
	int				getweight() const;
	bool			iscountable() const { return geti().is(Countable); }
	bool			ismagical() const { return getenchant()!=0; }
	void			setcount(int v);
};
struct itema : adat<item*> {
	void			select(struct creature& source);
	void			match(wear_s wear, bool keep);
};
struct spella : adat<spell_s> {
	void			known(const struct creature& source);
	void			prepared(const struct creature& source);
};
struct wearable {
	item			wears[Elbows + 1];
	void			additem(item& v);
	void			equip(item& v);
	const char*		getwearname(wear_s id) const;
	bool			isitem(const void* pv) const;
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
struct spelli : nameable {
	char			level[3];
	duration_s		duration;
	range_s			range;
	dice			effect;
	spell_s			dispelled[4];
	bool			isdurable() const { return duration != Instant; }
	bool			isevil() const { return range == EnemyCreatureTouched; }
};
struct spellable {
	unsigned char	spells[LastSpell + 1];
};
struct treasure : adat<item> {
	void			add(item it);
	void			generate(char symbol);
	void			take();
};
struct weari {
	const char*		id;
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
	bool			attack(ability_s attack, int ac, int bonus) const;
	void			choose(const slice<chooseoption>& options);
	void			clear();
	void			cast(spell_s spell);
	void			create(class_s type, gender_s gender);
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
