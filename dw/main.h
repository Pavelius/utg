#include "avatarable.h"
#include "crt.h"
#include "flagable.h"
#include "gender.h"
#include "recordset.h"
#include "utg.h"

#pragma once

enum ability_s : unsigned char {
	HP, Damage, Armor, Pierce,
	Strenght, Dexterity, Constitution, Intellegence, Wisdow, Charisma,
};
enum race_s : unsigned char {
	Human, Dwarf, Elf, Halfling,
};
enum tag_s : unsigned char {
	Arrows, CharismaMinus, Clumsy, Dangerous, Forceful, IgnoreArmor, Messy, NeedArrows, Precise, Ration, Reload, Slow, Stun, Thrown, TwoHanded,
	HealDebility, HealPoison,
	UsableGear,
	Hand, Close, Reach, Near, Far,
};
enum itemuf_s : unsigned char {
	HooksAndSpikes, Sharp, PerfectlyWeigthed, SerratedEdges, Glows, Huge, Versatile, WellCrafted,
};
enum wear_s : unsigned char {
	Head, Body, RightHand, LeftHand, RightRing, LeftRing, Legs,
	Backpack, LastBackpack = Backpack + 16,
};
enum move_s : unsigned char {
	HackAndSlash, Volley, DefyDanger, DefyPoison, Defend, SpoutLore, DiscernRealities, Parley,
	Aid, Interfere, LastBreath, Encumbrance,
	MakeCamp, TakeWatch, UndertakeAPerilousJourney, LevelUp, EndOfSession,
	Carouse, Supply, Recover, Recruit, OutstandingWarrants, Bolster,
};
enum action_s : unsigned char {
	SufferDamage, InflictDamage,
	UseAmmo, UseGear, UseRation, ForgetSpell,
	BreakItem, BreakObject,
};
enum variant_s : unsigned char {
	NoVariant,
	Ability, Action, Class, Diety, Item, Move, Pack, Race, StartEquipment, Tag,
};
enum result_s : unsigned char {
	Fail, PartialSuccess, Success, CriticalSuccess,
};

typedef flagable<1> itemufa;
typedef flagable<1 + Far / 8> taga;
typedef flagable<1 + Carouse / 8> movea;
typedef flagable<1 + Halfling / 8> racea;
typedef char abilitya[Charisma + 1];

struct packi {
	const char*		id;
	variants		elements;
	void			getinfo(stringbuilder& sb) const;
};
struct abilityi {
	const char*		id;
};
struct alignmenti {
	const char*		id;
};
struct racei {
	const char*		id;
};
struct classi {
	const char*		id;
	flaga			gender;
	flaga			race;
	flaga			alignment;
	ability_s		abilities[6];
};
struct dietyi {
	const char*		id;
};
struct startequipmenti {
	const char*		id;
	unsigned char	type;
	variants		elements;
};
struct tagi {
	const char* id;
};
struct actioni {
	const char*		id;
};
struct resulti {
	const char*		name;
	int				options;
	variants		effect;
};
struct movei {
	const char*		id;
	ability_s		ability;
	resulti			results[4];
};
struct moveable {
	movea			moves;
	char			forward;
	bool			is(move_s v) const { return moves.is(v); }
};
struct nameable {
	const char*		id;
	race_s			race;
	gender_s		gender;
public:
	void			act(const char* format, ...) { actv(format, xva_start(format)); }
	void			actv(const char* format, const char* format_param);
	gender_s		getgender() const { return gender; }
	const char*		getname() const { return getnm(id); }
	race_s			getrace() const { return race; }
};
struct itemi : nameable, moveable {
	wear_s			slot;
	taga			tags;
	racea			need;
	char			coins;
	char			weight, damage, pierce, armor, uses, heal;
};
union item {
	unsigned u;
	short unsigned w[2];
	struct {
		unsigned char type;
		unsigned char signature : 1;
		unsigned char used : 3;
		itemufa feats;
	};
	constexpr item() : u(0) {}
	constexpr item(unsigned char type) : type(0), signature(0), used(0), feats() {}
	constexpr const itemi& geti() const { return bsdata<itemi>::elements[type]; }
	constexpr bool is(itemuf_s v) const { return feats.is(v); }
	constexpr bool is(tag_s v) const { return geti().tags.is(v); }
};
typedef item weara[LastBackpack + 1];
struct statable : moveable {
	abilitya		abilities;
	static result_s last_result;
	static int		last_roll;
	static result_s rollv(int bonus);
};
class creature : public nameable, public avatarable, public statable {
	unsigned char	alignment;
	unsigned char	type, diety;
	statable		basic;
	void			choose_equipment();
	void			random_ability();
public:
	explicit operator bool() const { return getavatar()[0] != 0; }
	void			generate();
	int				get(ability_s v) const { return abilities[v]; }
	int				getbonus(ability_s v) const { return abilities[v] / 2 - 5; }
	void			getinfo(stringbuilder& sb) const;
};
class gamei {
	char			bolster;
public:
	int				getbolster() const { return bolster; }
	void			usebolster() { bolster--; }
};
inline int			d100() { return rand() % 100; }