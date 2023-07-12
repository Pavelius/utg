#include "answers.h"
#include "ability.h"
#include "crt.h"
#include "flagable.h"
#include "pathfind.h"
#include "point.h"
#include "nameable.h"
#include "script.h"
#include "strategy.h"
#include "tag.h"
#include "variant.h"

const int hms = 8;

enum planet_trait_s : unsigned char {
	NoTrait, Cultural, Hazardous, Industrial,
};
enum tag_s : unsigned char {
	IgnorePlanetaryShield, IgnoreSpaceCannon, IgnoreDirectHit, PlanetaryShield, RepairSustainDamage, SustainDamage,
	DeepSpaceCannon,
	CombatBonusToOthers, BuyCombatBonusToOther, ProduceInfantry, HeavyHits, AdditionalHitOn9n10,
	AddPlanetResourceToProduction
};
enum racef_s : unsigned char {
	Assimilate, Fragile, Unrelenting
};
enum flag_s : unsigned char {
	Exhaust
};
enum indicator_s : unsigned char {
	TradeGoods, Commodities, Resources, Influence,
	CommandToken, FleetToken, StrategyToken, TacticToken,
	VictoryPoints,
};
enum tile_s : unsigned char {
	NoSpecialTile, WormholeAlpha, WormholeBeta, AsteroidField, Nebula, Supernova, GravityRift,
};
enum color_s : unsigned char {
	NoTech, Red, Green, Blue, Yellow,
};
enum tech_s : unsigned char {
	PlasmaScoring, MagenDefenseGrid, DuraniumArmor, AssaultCannon,
	NeuralMotivator, DacxiveAnimators, HyperMetabolism, X89BacterialWeapon,
	AntimassDeflectors, GravityDriver, FleetLogistics, LightWaveDeflector,
	SarweenTools, GravitonLaserSystem, TransitDiodes, IntegratedEconomy
};
enum unit_type_s : unsigned char {
	GroundForces, Ships, Structures,
};
typedef flagable<4> taga;
typedef flagable<8> techa;
struct colori : nameable {
};
struct tilei : nameable {
};
struct indicatori : nameable {
};
struct planet_traiti : nameable {
};
struct unit_typei : nameable {
};
struct racefi : nameable {
};
struct actioncard;
struct card;
struct planeti;
struct playeri;
struct systemi;
struct troop;
struct uniti;
struct entity : nameable {
	playeri*		player;
	entity*			location;
	constexpr explicit operator bool() const { return id != 0; }
	void			add(answers& an);
	void			clear();
	void			event(int type, const char* format) const;
	int				get(ability_s v) const;
	int				get(indicator_s v) const;
	const actioncard* getactioncard() const;
	const char*		getid() const;
	playeri*		getenemy() const;
	const char*		getname() const;
	planeti*		getplanet() const;
	int				getproduction() const;
	color_s			getspeciality() const;
	int				getsumary(ability_s v) const;
	int				getsumary(unit_type_s v) const;
	systemi*		getsystem() const;
	planet_trait_s	gettrait() const;
	const uniti*	getunit() const;
	void			hit();
	bool			is(flag_s v) const;
	bool			is(tag_s v) const;
	void			remove(flag_s v);
	troop*			sibling(troop* pb) const;
	void			startcombat();
	void			set(flag_s v);
};
struct uniti : nameable {
	char			abilities[Capacity + 1];
	unit_type_s		type;
	taga			tags;
	playeri*		race;
	const uniti*	replace;
	static uniti*	last;
	int				getcost() const { return abilities[Cost]; }
	int				getweight() const { return abilities[Cost] * 2 / imax(1, (int)abilities[CostCount]); }
	void			placement(int count, bool updateui = true) const;
	bool			stackable() const { return abilities[CostCount] > 1; }
};
struct planeti : entity {
	planet_trait_s	trait;
	color_s			speciality;
	char			frame, resources, influence;
	flagable<1>		flags;
	void			exhaust();
	int				get(indicator_s v) const;
	void			paint(unsigned flags) const;
};
extern planeti* last_planet;
struct playera : adat<playeri*, 6> {
	playeri*		choose(const char* title);
	void			filter(const playeri* object, bool keep);
	void			shuffle() { zshuffle(data, count); }
};
struct systemi : entity {
	flagable<4>		activated;
	playeri*		home;
	char			special_index;
	tile_s			special;
	pathfind::indext index;
	static systemi*	active;
	static systemi*	last;
	explicit operator bool() const { return index != pathfind::Blocked; }
	static void		blockenemy(const playeri* player);
	static void		blockmove();
	planeti*		getbestplanet() const;
	int				getcapacity(bool include_docks = true) const;
	bool			isactivated(const playeri* p) const;
	bool			isplay() const { return index != pathfind::Blocked; }
	void			limitcapacity();
	static systemi*	findbyindex(pathfind::indext i);
	static void		markzerocost(const playeri* player);
	bool			movestop() const;
	bool			movethrought() const;
	void			paint() const;
	void			placement(const uniti* unit, playeri* player);
	void			placement(variants source, playeri* player);
	void			setactivate(const playeri* p, bool active);
};
struct requirement {
	char			required[4]; // RGBY
	bool			match(const requirement& e) const;
};
struct techi : nameable {
	color_s			color;
	char			required[4]; // RGBY
	void			getinfo(stringbuilder& sb) const;
	static void		getinfo(const void* object, stringbuilder& sb) { ((techi*)object)->getinfo(sb); }
	bool			match(const requirement& e) const;
};
struct troop : entity {
	flagable<1>		flags;
	static troop*	create(const char* id, playeri* player);
	static troop*	create(const uniti* unit, playeri* player, entity* location);
	void			clear() { memset(this, 0, sizeof(*this)); }
	void			hit();
	void			paint(unsigned flags) const;
	void			produce(const uniti* unit) const;
	static void		updateui();
};
extern troop* lasttroop;
struct card : entity {
	void			paint() const;
};
struct entitya : public adat<entity*> {
	void			activated(const playeri* player, bool keep);
	void			addu(entity* v);
	void			addreach(const systemi* system, int range);
	entity*			choose(const char* title, const char* cancel = 0, int choose_mode = 0) const;
	void			filter(const entity* object, bool keep);
	entity*			getbest(indicator_s v) const;
	int				getcap() const;
	int				getsummary(ability_s v) const;
	int				getsummary(indicator_s v) const;
	int				getsummary(unit_type_s v) const;
	int				getsummary(const uniti* pv) const;
	void			grouplocation(const entitya& source);
	void			groupsystem(const entitya& source);
	void			ingame();
	bool			have(entity* v) const { return find(v) != -1; }
	void			match(ability_s id, int value, bool keep);
	void			match(const playeri* player, bool keep);
	void			match(const systemi* system, bool keep);
	void			match(unit_type_s type, bool keep);
	void			match(planet_trait_s value, bool keep);
	void			match(color_s value, bool keep);
	void			match(flag_s value, bool keep);
	void			match(indicator_s value, bool keep);
	void			matchload(bool keep);
	void			matchmove(int mode, bool keep);
	void			matchrange(int range, bool keep);
	entity*			random() const;
	entity*			pick();
	void			select(array& source);
	void			select(answers& an);
	void			select(const entity* location);
	void			select(const playeri* player, const entity* system, unit_type_s type);
	void			select(const playeri* player, const entity* location);
	void			selectcards(const playeri* player);
	void			selectplanets(const systemi* system);
	void			sortunit();
};
struct agendai : nameable {
	variants		target;
	variants		yes, no;
};
struct objectivei : nameable {
	char			stage, value;
	variants		condition;
};
struct component : nameable {
	variant			parent;
	const char*		trigger;
	variants		use;
	bool			isallow() const;
};
struct actioncard : component {
	char			count;
};
class analize {
	pathfind::indext data[hms * hms];
public:
	void			clear();
	int				getweight(pathfind::indext i) const;
	void			setweight(pathfind::indext i, int v);
};
struct army {
	playeri*		owner;
	entitya			units;
	bool			reatreat;
	static army*	last;
	static int		round;
	void			clear() { memset(this, 0, sizeof(*this)); }
	void			choose(const char* id);
	void			hit(int value);
	int				roll(ability_s id, ability_s id_count) const;
};
struct prototype {
	uniti			units[10];
};
struct unitupgrade : uniti, requirement {
};

struct gamei {
	playeri*		speaker;
	playera			players, origin_players;
	indicator_s		indicator;
	static systemi* choosesystem(const entitya& source);
	static troop*	choosetroop(const entitya& source);
	static void		focusing(const entity* p);
	static void		initialize();
	void			limitcapacity();
	void			prepare();
	void			prepareui();
	void			play();
	static int		rate(indicator_s need, indicator_s currency, int count);
	void			sortbyinitiative();
	static void		updatecontrol();
	static void		updateui();
};
extern gamei game;

struct playeri : nameable {
	char			indicators[VictoryPoints + 1];
	techa			tech, tech_used;
	flagable<4>		race;
	char			commodities;
	strategyi*		strategy;
	variants		troops, startup;
	bool			use_strategy;
	bool			pass_action_phase;
	static playeri* human;
	void			actv(const char* format, const char* format_param) const;
	void			add(indicator_s v, int i);
	void			apply(const variants& source);
	void			assign(variants source);
	bool			canbuild(const uniti* player) const;
	bool			is(tech_s v) const { return tech.is(v); }
	bool			is(racef_s v) const { return race.is(v); }
	bool			ishuman() const { return this == human; }
	int				get(indicator_s v) const { return indicators[v]; }
	void			getadvance(requirement& result) const;
	int				getcards() const;
	systemi*		gethome() const;
	int				getindex() const;
	int				getinitiative() const;
	void			getinfo(stringbuilder& sb) const;
	int				getplanetsummary(indicator_s v) const;
	int				getsummary(const uniti* type) const;
	int				gettechs() const;
	const uniti*	getunit(int index) const;
	void			sayspeech(const char* id) const;
	void			set(indicator_s v, int i) { indicators[v] = i; }
	void			setcontrol(planeti* p);
};
extern playeri*	player;

inline point i2h(pathfind::indext i) { return {(short)(i % hms), (short)(i / hms)}; }
inline pathfind::indext	h2i(point v) { return v.y * hms + v.x; }

extern entitya querry;