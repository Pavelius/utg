#include "ability.h"
#include "advancement.h"
#include "avatarable.h"
#include "charname.h"
#include "crt.h"
#include "dice.h"
#include "flagable.h"
#include "gender.h"
#include "namenpc.h"
#include "quest.h"
#include "recordset.h"
#include "result.h"
#include "tag.h"
#include "utg.h"

#pragma once

template<typename T> struct bsmeta;

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
	Ability, Action, Advancement, Alignment, Class, Creature,
	Diety, Front, Gender, Item, Menu, Move, Pack, Race, Tag, Widget
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
struct alignmenti {
	const char*		id;
};
struct racei {
	const char*		id;
};
struct classi {
	const char*		id;
	ability_s		abilities[6];
	int				damage;
};
struct dietyi {
	const char*		id;
};
struct actioni {
	const char*		id;
};
struct movei {
	const char*		id;
	ability_s		ability;
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
	constexpr item(unsigned char type) : type(type), signature(0), used(0), feats() {}
	constexpr explicit operator bool() const { return u != 0; }
	constexpr const itemi& geti() const { return bsdata<itemi>::elements[type]; }
	constexpr bool	is(itemuf_s v) const { return feats.is(v); }
	constexpr bool	is(tag_s v) const { return geti().tags.is(v); }
};
typedef item weara[LastBackpack + 1];
struct statable : moveable {
	abilitya		abilities;
	void			copy(statable& v) { *this = v; }
	void			apply_ability(int v);
	void			update_player();
};
class wearable {
	item			wears[LastBackpack + 1];
public:
	bool			additem(const item& it);
};
class creature : public namenpc, public avatarable, public statable, public wearable {
	unsigned char	alignment, type, diety;
	race_s			race;
	statable		basic;
	char			hp;
	void			apply_advance();
	void			choose_avatar();
	void			choose_abilities();
	void			choose_name();
	void			finish();
	void			random_ability();
	void			update();
	void			update_class(classi& e);
	friend bsmeta<creature>;
public:
	explicit operator bool() const { return isvalidname(); }
	void			generate();
	int				get(ability_s v) const { return abilities[v]; }
	static const char* getavatarst(const void* object);
	int				getbonus(ability_s v) const { return abilities[v] / 2 - 5; }
	const classi&	getclass() const { return bsdata<classi>::elements[type]; }
	dice			getdamage() const;
	void			getinfo(stringbuilder& sb) const;
	static void		getinfost(const void* object, stringbuilder& sb) { ((creature*)object)->getinfo(sb); }
	static void		getpropertyst(const void* object, variant v, stringbuilder& sb);
	bool			ismatch(variant v) const;
};
class gamei {
	char			bolster;
public:
	int				getbolster() const { return bolster; }
	void			usebolster() { bolster--; }
};
inline int			d100() { return rand() % 100; }

VKIND(race_s, Race)
VKIND(gender_s, Gender)