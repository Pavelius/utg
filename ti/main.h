#include "answers.h"
#include "crt.h"
#include "flagable.h"
#include "point.h"
#include "pathfind.h"
#include "script.h"
#include "tag.h"
#include "variant.h"

const int hms = 8;

enum ability_s : unsigned char {
	Cost, CostCount,
	Combat, CombatCount, Bombardment, BombardmentCount,
	AntiFighterBarrage, AntiFighterBarrageCount, SpaceCannon, SpaceCannonCount,
	Move, Production, Reinforcement, Capacity,
};
enum planet_trait_s : unsigned char {
	NoTrait, Cultural, Hazardous, Industrial,
};
enum tag_s : unsigned char {
	IgnorePlanetaryShield, IgnoreSpaceCannon, PlanetaryShield, RepairSustainDamage, SustainDamage,
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
typedef void(*fnanswer)(answers& an);
typedef void(*fnapplyanswer)();
struct abilityi {
	const char*		id;
};
struct colori {
	const char*		id;
};
struct tilei {
	const char*		id;
};
struct nameable {
	const char*		id;
	const char*		getname() const { return getnm(id); }
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
struct playeri;
struct systemi;
struct troop;
struct planeti;
struct entity : nameable {
	playeri*		player;
	entity*			location;
	constexpr explicit operator bool() const { return id != 0; }
	void			clear() { memset(this, 0, sizeof(*this)); }
	static int		fight(int chance, int count = 0, int reroll = 0);
	int				get(ability_s v) const;
	int				get(indicator_s v) const;
	const char*		getid() const;
	const char*		getname() const { return getnm(getid()); }
	planeti*		getplanet() const;
	color_s			getspeciality() const;
	systemi*		getsystem() const;
	planet_trait_s	gettrait() const;
	bool			is(flag_s v) const;
	troop*			sibling(troop* pb) const;
};
struct uniti : nameable {
	taga			tags;
	char			abilities[Capacity + 1];
	unit_type_s		type;
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
};
struct systemi : entity {
	flagable<4>		activated;
	playeri*		home;
	char			special_index;
	tile_s			special;
	pathfind::indext index;
	static systemi*	last;
	bool			isactivated(const playeri* p) const;
	bool			isplay() const { return index != pathfind::Blocked; }
	planeti*		getbestplanet() const;
	void			paint() const;
	void			placement(const uniti* unit, playeri* player);
	void			placement(variants source, playeri* player);
	void			setactivate(const playeri* p, bool active);
};
struct techi {
	const char*		id;
	color_s			color;
	char			required[4]; // RGBY
};
struct triggeri {
	const char*		id;
};
struct troop : entity {
	const uniti*	type;
	static troop*	add(const char* id, playeri* player);
	static troop*	add(const uniti* unit, playeri* player, entity* location);
	void			clear() { memset(this, 0, sizeof(*this)); }
	const char*		getname() const { return getnm(type->id); }
	int				getstackcount() const;
	troop*			getstackholder();
	void			paint() const;
};
struct entitya : public adat<entity*> {
	void			addu(entity* v);
	entity*			choose(const char* title) const;
	void			filter(const entity* object, bool keep);
	int				fight(ability_s power, ability_s count);
	entity*			getbest(indicator_s v) const;
	void			grouplocation(const entitya& source);
	bool			have(entity* v) const { return find(v) != -1; }
	void			match(const playeri* player, bool keep);
	void			match(const systemi* system, bool keep);
	void			match(planet_trait_s value, bool keep);
	void			match(color_s value, bool keep);
	void			match(flag_s value, bool keep);
	void			match(indicator_s value, bool keep);
	void			select(array& source);
	void			select(const playeri* player, const entity* location);
	void			selectplanets(const systemi* system);
};
struct combat {
	entitya			attacker, defender;
};
struct poweri {
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
class galaxy {
	unsigned short	tiles[hms * hms];
public:
	void			cleargalaxy();
	systemi*		get(pathfind::indext) const;
	void			set(pathfind::indext, const systemi* v);
};
class gamestring : public stringbuilder {
	void			addidentifier(const char* identifier) override;
public:
	gamestring(const stringbuilder& v) : stringbuilder(v) {}
};
struct choosestep {
	const char*		id;
	fnanswer		panswer;
	fnapplyanswer	papply;
	void			run() const;
	static void		run(const char* id);
};
struct component : nameable {
	variant			parent;
	choosestep*		trigger;
	variants		use;
	bool			isallow() const;
};
struct actioncardi : component {
	char			count;
};
struct gamei {
	playeri*		speaker;
	playeri*		human;
	playeri*		active;
	playera			players;
	indicator_s		indicator;
	static void*	result;
	static int		options;
	void			focusing(const entity* p);
	static void		initialize();
	void			prepare();
	void			prepareui();
	static int		rate(indicator_s need, indicator_s currency, int count);
	static void		updateui();
};
struct playeri : nameable {
	char			indicators[VictoryPoints + 1];
	techa			tech, tech_used;
	uniti			units[10];
	char			commodities;
	strategyi*		strategy;
	variants		troops;
	bool			use_strategy;
	static playeri* last;
	static playeri* human;
	void			add(indicator_s v, int i);
	void			apply(const variants& source);
	bool			is(tech_s v) const { return tech.is(v); }
	bool			ishuman() const { return this == human; }
	int				get(indicator_s v) const { return indicators[v]; }
	systemi*		gethome() const;
	void			set(indicator_s v, int i) { indicators[v] = i; }
	void			setcontrol(planeti* p);
};
inline point		i2h(pathfind::indext i) { return {(short)(i % hms), (short)(i / hms)}; }
inline pathfind::indext	h2i(point v) { return v.y * hms + v.x; }
extern gamei		game;
extern entitya		querry;