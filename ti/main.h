#include "crt.h"
#include "entity.h"
#include "pathfind.h"
#include "point.h"
#include "script.h"
#include "strategy.h"
#include "tech.h"
#include "variant.h"
#include "unit.h"

const int hms = 8;

enum planet_trait_s : unsigned char {
	NoTrait, Cultural, Hazardous, Industrial,
};
enum racef_s : unsigned char {
	Assimilate, Fragile, Unrelenting
};
enum tile_s : unsigned char {
	NoSpecialTile, WormholeAlpha, WormholeBeta, AsteroidField, Nebula, Supernova, GravityRift,
};
typedef flagable<8> techa;
struct tilei : nameable {
};
struct planet_traiti : nameable {
};
struct unit_typei : nameable {
};
struct racefi : nameable {
};
struct actioncard;
struct planeti;
struct playeri;
struct systemi;
struct troop;
struct uniti;
struct planeti : entity {
	planet_trait_s	trait;
	color_s			speciality;
	char			frame, resources, influence;
	flagable<1>		flags;
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
	playeri*		home;
	char			special_index;
	tile_s			special;
	pathfind::indext index;
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
extern systemi* last_system;
extern systemi* active_system;
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
extern troop* last_troop;
struct entitya : public adat<entity*> {
	void			activated(const playeri* player, bool keep);
	void			addu(entity* v);
	void			addreach(const systemi* system, int range);
	entity*			choose(const char* title, const char* cancel = 0, int choose_mode = 0) const;
	void			filter(const entity* object, bool keep);
	entity*			getbest(indicator_s v) const;
	int				getcap() const;
	int				getproductioncost() const;
	int				getsummary(ability_s v) const;
	int				getsummary(indicator_s v) const;
	int				getsummary(unit_type_s v) const;
	int				getsummary(const uniti* pv) const;
	int				getfight() const;
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
	void			match(tag_s value, bool keep);
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
	void			selectplanets(const systemi* system);
	void			sortunit();
};
struct agendai : nameable {
	variants		target;
	variants		yes, no;
};
struct actioncard : nameable {
	variant			parent;
	variants		use;
	const char*		trigger;
	char			count;
	bool			isallow() const;
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
struct unitupgrade : uniti, requirement {
};
struct gamei {
	playeri*		speaker;
	playera			players, origin_players;
	indicator_s		indicator;
	int				finale_score;
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