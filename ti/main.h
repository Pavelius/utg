#include "crt.h"
#include "flagable.h"
#include "tag.h"
#include "variant.h"

enum ability_s : unsigned char {
	Cost, CostCount,
	Combat, CombatCount, Bombardment, BombardmentCount,
	AntiFighterBarrage, AntiFighterBarrageCount, SpaceCannon, SpaceCannonCount,
	Move, Production, Reinforcement, Capacity,
};
enum tag_s : unsigned char {
	IgnorePlanetaryShield, PlanetaryShield, SustainDamage
};
enum indicator_s : unsigned char {
	TradeGoods, Commodities, Resources, Influence,
};
enum wormhole_s : unsigned char {
	NoHole, WormholeAlpha, WormholeBeta
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
struct nameable {
	const char*		id;
};
struct unit_typei {
	const char*		id;
};
struct playeri;
struct orderable : nameable {
	playeri*		player;
	orderable*		location;
	taga			tags;
	constexpr explicit operator bool() const { return player != 0; }
	void			clear() { memset(this, 0, sizeof(*this)); }
	static int		fight(int chance, int count = 0, int reroll = 0);
	bool			is(tag_s v) const { return tags.is(v); }
	int				get(ability_s v) const;
	int				get(indicator_s v) const;
	void			set(tag_s v) { return tags.set(v); }
};
struct uniti : nameable {
	taga			tags;
	char			abilities[Capacity + 1];
	unit_type_s		type;
};
struct planeti : orderable {
	char			resources, influences;
	int				get(indicator_s v) const;
};
struct indicatora {
	char			indicators[Influence + 1];
};
struct playeri : nameable, indicatora {
	techa			tech, tech_used;
	uniti			units[10];
	bool			is(tech_s v) const { return tech.is(v); }
};
struct systemi : orderable {
};
struct techi {
	const char*		id;
	color_s			color;
	char			required[4]; // RGBY
};
struct troop : orderable {
	const uniti*	type;
	void			create(const char* id);
};
struct orderablea : public adat<orderable*> {
	int				fight(ability_s power, ability_s count);
	void			select(array& source);
	void			select(const playeri* player, const orderable* location);
};
struct combat {
	orderablea		attacker, defender;
};