#include "answers.h"
#include "choosestep.h"
#include "crt.h"
#include "flagable.h"
#include "pathfind.h"
#include "point.h"
#include "nameable.h"
#include "script.h"
#include "tag.h"
#include "variant.h"

const int hms = 8;

enum ability_s : unsigned char {
	Cost, CostCount,
	Combat, CombatCount, Bombardment, BombardmentCount,
	AntiFighterBarrage, AntiFighterBarrageCount, SpaceCannon, SpaceCannonCount,
	Move, Production, Reinforcement, MaximumInOneLocation, CapacityShips, Capacity,
};
enum planet_trait_s : unsigned char {
	NoTrait, Cultural, Hazardous, Industrial,
};
enum tag_s : unsigned char {
	IgnorePlanetaryShield, IgnoreSpaceCannon, PlanetaryShield, RepairSustainDamage, SustainDamage,
	AddPlanetResourceToProduction
};
enum racef_s : unsigned char {
	Assimilate, Fragile,
};
enum flag_s : unsigned char {
	Exhaust
};
enum indicator_s : unsigned char {
	TradeGoods, Commodities, Resources, Influence,
	CommandToken, FleetToken, StrategyToken, TacticToken,
	VictoryPoints
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
	SarweenTools, GravitonLaserSystem, TransitDiodes, IntegratedEconomy,
	CruiserII, DreadnoughtII, DestroyerII, PDSII, CarrierII, FighterII, InfantryII, SpaceDockII,
	WarSunTech,
};
enum unit_type_s : unsigned char {
	GroundForces, Ships, Structures,
};
typedef flagable<4> taga;
typedef flagable<8> techa;
struct abilityi {
	const char*		id;
};
struct colori {
	const char*		id;
};
struct tilei {
	const char*		id;
};
struct indicatori {
	const char*		id;
};
struct planet_traiti {
	const char*		id;
};
struct unit_typei {
	const char*		id;
};
struct racefi {
	const char*		id;
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
	static int		fight(int chance, int count = 0, int reroll = 0);
	int				get(ability_s v) const;
	int				get(indicator_s v) const;
	const actioncard* getactioncard() const;
	const char*		getid() const;
	const char*		getname() const;
	planeti*		getplanet() const;
	int				getproduction() const;
	color_s			getspeciality() const;
	int				getsumary(ability_s v) const;
	int				getsumary(unit_type_s v) const;
	systemi*		getsystem() const;
	planet_trait_s	gettrait() const;
	const uniti*	getunit() const;
	bool			is(flag_s v) const;
	troop*			sibling(troop* pb) const;
};
struct uniti : nameable {
	taga			tags;
	char			abilities[Capacity + 1];
	unit_type_s		type;
	static uniti*	last;
	int				getcost() const { return abilities[Cost]; }
	void			placement(int count, bool updateui = true) const;
	bool			stackable() const { return abilities[CostCount] > 1; }
};
struct planeti : entity {
	planet_trait_s	trait;
	color_s			speciality;
	char			frame;
	char			resources, influence;
	flagable<1>		flags;
	static planeti*	last;
	void			exhaust();
	int				get(indicator_s v) const;
	void			paint(unsigned flags) const;
};
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
struct techi {
	const char*		id;
	color_s			color;
	char			required[4]; // RGBY
	void			getinfo(stringbuilder& sb) const;
	static void		getinfo(const void* object, stringbuilder& sb) { ((techi*)object)->getinfo(sb); }
};
struct triggeri {
	const char*		id;
};
struct troop : entity {
	static troop*	last;
	static troop*	create(const char* id, playeri* player);
	static troop*	create(const uniti* unit, playeri* player, entity* location);
	void			clear() { memset(this, 0, sizeof(*this)); }
	void			paint(unsigned flags) const;
	void			produce(const uniti* unit) const;
};
struct card : entity {
	void			paint() const;
};
struct entitya : public adat<entity*> {
	void			activated(const playeri* player, bool keep);
	void			addu(entity* v);
	void			addreach(const systemi* system, int range);
	entity*			choose(const char* title, const char* cancel = 0) const;
	void			filter(const entity* object, bool keep);
	int				fight(ability_s power, ability_s count);
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
struct combat {
	entitya			attacker, defender;
};
struct agendai : nameable {
	variants		target;
	variants		yes, no;
};
struct strategyi : nameable {
	int				initiative;
	variants		primary, secondary;
};
struct objectivei : nameable {
	char			stage, value;
	variants		condition;
};
class gamestring : public stringbuilder {
	void			addidentifier(const char* identifier) override;
public:
	gamestring(const stringbuilder& v) : stringbuilder(v) {}
};
struct component : nameable {
	variant			parent;
	choosestep*		trigger;
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
struct gamei {
	playeri*		speaker;
	playeri*		active;
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
struct playeri : nameable {
	char			indicators[VictoryPoints + 1];
	techa			tech, tech_used;
	uniti			units[10];
	char			commodities;
	strategyi*		strategy;
	variants		troops, startup;
	bool			use_strategy;
	bool			pass_action_phase;
	static playeri* last;
	static playeri* human;
	void			add(indicator_s v, int i);
	void			apply(const variants& source);
	void			assign(variants source);
	bool			canbuild(const uniti* player) const;
	bool			is(tech_s v) const { return tech.is(v); }
	bool			ishuman() const { return this == human; }
	int				get(indicator_s v) const { return indicators[v]; }
	int				getcards() const;
	systemi*		gethome() const;
	int				getinitiative() const;
	void			getinfo(stringbuilder& sb) const;
	int				getplanetsummary(indicator_s v) const;
	int				getsummary(const uniti* type) const;
	int				gettechs() const;
	void			set(indicator_s v, int i) { indicators[v] = i; }
	void			setcontrol(planeti* p);
};
inline point		i2h(pathfind::indext i) { return {(short)(i % hms), (short)(i / hms)}; }
inline pathfind::indext	h2i(point v) { return v.y * hms + v.x; }
extern gamei		game;
extern entitya		querry;