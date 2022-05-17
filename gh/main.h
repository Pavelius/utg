#include "ability.h"
#include "answers.h"
#include "condition.h"
#include "crt.h"
#include "deck.h"
#include "flagable.h"
#include "gender.h"
#include "menu.h"
#include "pathfind.h"
#include "point.h"
#include "script.h"
#include "variant.h"

enum duration_s : unsigned char {
	Instant, Infinite, Round, Use
};
enum area_s : unsigned char {
	NoArea, Slash, Circle, Ray, Splash, Spray,
};
enum special_s : unsigned char {
	AttackOneTarget, EnemyAttackYouInsteadNearestAlly, GainExpForRetaliate, GainExpForTarget,
	TargetEnemyMoveThrought,
};
enum statistic_s : unsigned char {
	Moved, Attacked, Looted,
	MonstersKilled, ItemsUsed,
};
enum modifier_s : unsigned char {
	Experience, Pierce, Range, Target,
};
enum action_s : unsigned char {
	Apply,
	Shield, Retaliate,
	Move, Attack, Push, Pull, Heal, DisarmTrap, Loot, Kill,
	Bless, Curse,
	RecoverDiscarded,
	TrapDamage, TrapPoisonDamage,
	Discard
};
enum game_propery_s : unsigned char {
	Reputation, Prosperty, Coins, Donate,
};
enum state_s : unsigned char {
	Disarmed, Immobilize, Wound, Muddle, Poison, Invisibility, Stun, Strenght,
	Jump, Fly, Mirrored, Hostile, UseUpper, UseLower, Elite,
};
enum element_s : unsigned char {
	Fire, Ice, Air, Earth, Light, Dark, AnyElement,
};
//typedef short unsigned indext;
typedef flagable<2> statef;
typedef flagable<1 + TargetEnemyMoveThrought / 8> featf;
typedef flagable<1> elementf;
typedef flagable<4>	playerf;
const int				hms = 32;
inline point			i2h(pathfind::indext i) { return {(short)(i % hms), (short)(i / hms)}; }
inline pathfind::indext	h2i(point v) { return v.y * hms + v.x; }
struct playeri;
struct actioni {
	const char*			id;
};
struct areai {
	const char*			id;
};
struct modifieri {
	const char*			id;
};
struct speciali {
	const char*			id;
};
struct combatcardi {
	const char*			id;
	char				bonus, count, next, shuffle;
	variants			feats;
	bool				better(const combatcardi& e) const { return bonus > e.bonus; }
};
struct combatdeck : deck {
	void				add(const char* id);
	void				discard(combatcardi* p) { deck::discard(p - bsdata<combatcardi>::elements); }
	void				initialize();
	combatcardi*		take() { return bsdata<combatcardi>::elements + deck::take(); }
	combatcardi*		takebad(int count);
	combatcardi*		takegood(int count);
};
struct playercardi {
	const char*			id;
	int					initiative;
	char				level;
	playeri*			owner;
	variants			upper, lower;
	variants			getabilities(int n) const { return n ? lower : upper; }
	void				getinfo(stringbuilder& sb) const;
};
struct playerdeck : deck {
	void				addcards(const char* id, int level);
	playercardi*		choose(const char* title, bool needremove, fnevent proc = 0);
	void				discard(playercardi* p) { deck::discard(p - bsdata<playercardi>::elements); }
	playercardi*		take() { return bsdata<playercardi>::elements + deck::take(); }
};
struct playeri {
	const char*			id;
	gender_s			gender;
	char				level, exp, coins;
	playercardi*		cards[2];
	short				health[10];
	playerdeck			hand, discard, lost;
	combatdeck			combat;
	static playeri*		last;
};
struct monstercardi {
	const char*			id;
	int					initiative;
	variants			abilities;
	bool				shuffle;
	void				getinfo(stringbuilder& sb) const;
};
struct cardtypei {
	const char*			id;
};
struct durationi {
	const char*			id;
};
struct elementi {
	const char*			id;
};
struct feati {
	const char*			id;
};
struct statei {
	const char*			id;
	bool				hostile;
};
struct trapi {
	const char*			id;
	char				damage;
	variants			feats;
	const char*			avatar;
};
struct summoni {
	const char*			id;
	char				hits, move, attack, range;
	variants			feats;
};
struct monsteri : summoni {
	char				level, elite;
};
struct nameable {
	variant				kind;
	void                act(const char* format, ...) const;
	gender_s			getgender() const;
	const char*         getname() const { return kind.getname(); }
};
struct tilei {
	const char*			id;
	point				size; // size of field in tiles
	int					priority;
	point				offset; // offset to upper left tile
	slice<point>		blocks; // blocked squares
	void				creating(point position, bool inverse) const;
};
struct eventi {
	struct action {
		int				stage;
		const char*		text;
		variants		consequences;
	};
	const char*			text;
	const char*			case1;
	const char*			case2;
	action				actions[8];
	void				clear();
	static bool			read(const char* url, array& source);
};
struct scenariotilei {
	const tilei*		type;
	point				position;
	bool				inverse;
	explicit operator bool() const { return type != 0; }
	void				updateui() const;
};
struct scenarioi {
	const char*			id;
	scenariotilei		tiles[32];
	point				starts[8];
	void				prepare(int stage) const;
};
struct action {
	action_s			type;
	char				bonus;
	char				modifiers[Target + 1];
	statef				you, opponent;
	elementf			gather, consume;
	area_s				area;
	char				area_size;
	void				clear();
	int					get(modifier_s v) const { return modifiers[v]; }
	bool				is(element_s v) const;
	const variant*		parse_condition(const variant* p, const variant* pe);
	const variant*		parse_modifier(const variant* p, const variant* pe);
};
struct activecardi {
	duration_s			type;
	playercardi*		card;
	playeri*			player;
	variants			effect;
	char				uses, uses_experience;
	constexpr explicit operator bool() const { return type != Instant; }
	static activecardi*	add(playeri* player, playercardi* card, variants effect);
	void				clear();
	void				discard();
	void				use();
};
class location {
	flagable<hms*hms>	walls;
public:
	bool				iswall(pathfind::indext i) const { return walls.is(i); }
	void				setpass(pathfind::indext i) { walls.remove(i); }
	void				setwall(pathfind::indext i) { walls.set(i); }
	void				setwalls();
};
class indexable {
	point				value;
public:
	static pathfind::indext	choosemove();
	void				fixattack(indexable& enemy) const;
	void				fixdamage(int damage) const;
	void				fixkill() const;
	void				fixmove(point hex) const;
	pathfind::indext	getindex() const { return h2i(value); }
	point				getposition() const { return value; }
	void				setposition(point v) { value = v; }
	void				setposition(pathfind::indext v) { value = i2h(v); }
};
class creaturei : public indexable {
	const void*			parent;
	char				level, hits;
	statef				state;
public:
	explicit operator bool() const { return parent != 0; }
	static creaturei*	add(const char* id, point position, bool elite = false);
	void				attack(creaturei& enemy, int bonus, int pierce = 0);
	void				choosecards();
	void				clear();
	void				damage(int v);
	combatdeck&			getcombatdeck() const;
	const char*			getid() const;
	int					getinitiative(int index = 0) const;
	int					gethp() const { return hits; }
	int					getmaximumhp() const;
	const summoni*		getmonster() const;
	int					getongoing(action_s v) const;
	playeri*			getplayer() const;
	bool				is(state_s v) const { return state.is(v); }
	bool				isplayer() const;
	void				kill();
	void				move(int bonus);
	void				play();
	void				updateui() const;
};
struct creaturea : adat<creaturei*> {
	void				select();
	void				sort();
};
namespace draw {
void					waitall();
}
struct gamei : public location {
	combatdeck			combat;
	int					dungeon_level;
	static void			checkinitiative();
	static void			choosecards();
	static void			initialize();
	static duration_s	getduration(variants source);
	static int			getrounds(variants source);
	static int			parse(variants source, action* pb);
	static void			playmoves();
	static void			playround();
	static void			setcamera(point pt);
	static void			updateui(void* parent, point position);
};
extern gamei			game;