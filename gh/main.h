#include "ability.h"
#include "answers.h"
#include "color.h"
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
enum target_s : unsigned char {
	TargetAttack, TargetSelf, TargetAlly, TargetEnemiesAround, TargetAlliesAround, TargetCreaturesAround,
};
enum modifier_s : unsigned char {
	Bonus, Experience, Pierce, Range, Target,
};
enum action_s : unsigned char {
	Shield, Retaliate,
	Move, Attack, Push, Pull, Heal, DisarmTrap, Loot, Kill,
	Bless, Curse,
	RecoverDiscard, RecoverLost,
	Discard
};
enum game_propery_s : unsigned char {
	Reputation, Prosperty, Coins, Donate,
};
enum state_s : unsigned char {
	Disarmed, Immobilize, Wound, Muddle, Poison, Invisibility, Stun, Strenght,
	Jump, Fly, Mirrored, Hostile, Elite,
};
enum element_s : unsigned char {
	Fire, Ice, Air, Earth, Light, Dark, AnyElement,
};
enum tile_s : unsigned char {
	Corridor, Coin,
};
typedef flagable<2> statef;
typedef flagable<1 + TargetEnemyMoveThrought / 8> featf;
typedef flagable<1> elementf;
typedef flagable<4>	playerf;
typedef char modifiera[Target + 1];
const int				hms = 32;
inline point			i2h(pathfind::indext i) { return {(short)(i % hms), (short)(i / hms)}; }
inline pathfind::indext	h2i(point v) { return v.y * hms + v.x; }
struct playeri;
class creaturei;
struct nameable {
	const char*			id;
};
struct actioni {
	typedef void (*fnevent)(int bonus);
	const char*			id;
	fnevent				proc;
};
struct areai {
	const char*			id;
	point				points[16];
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
	static playercardi*	last;
	variants			getabilities(int n) const { return n ? lower : upper; }
	void				getinfo(stringbuilder& sb) const;
	void				paint() const;
	void				paint_statistic() const;
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
	color				fore;
	static playeri*		last;
};
struct monstercardi {
	const char*			id;
	int					initiative;
	variants			abilities;
	bool				shuffle;
};
struct monsterdeck : nameable, deck {
	static monsterdeck&	get(const char* id);
	const monstercardi* current() { return count ? bsdata<monstercardi>::elements + data[0] : 0; }
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
struct targeti {
	const char*			id;
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
	int					get(action_s v) const;
	int					get(modifier_s v) const;
};
struct monsteri : summoni {
	char				level, elite;
	const char*			deck;
	const char*			getdeck() const { return deck ? deck : id; }
};
struct tilei {
	const char*			id;
	point				size; // size of field in tiles
	int					priority;
	point				offset; // offset to upper left tile
	slice<point>		blocks; // blocked squares
	void				creating(point position, bool inverse) const;
	bool				isdecoration() const { return priority >= 4; }
	void				updateui();
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
	const char*			type;
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
struct activecardi {
	duration_s			duration;
	creaturei*			target;
	playercardi*		card;
	char				uses;
	action_s			type;
	char				bonus;
	action_s			discard_action;
	constexpr explicit operator bool() const { return duration != Instant; }
	static activecardi*	add(creaturei* target, playercardi* card, duration_s duration, char uses, const slice<variant>& source);
	void				addsource(variant* p, variant* pe);
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
	pathfind::indext	value;
public:
	static pathfind::indext	choosemove();
	void				disappear() const;
	void				fixattack(indexable& enemy) const;
	void				fixdamage(int damage) const;
	void				fixgood(const char* format, ...) const;
	void				fixexperience(int value) const;
	void				fixheal(int value) const;
	void				fixmove(point hex) const;
	pathfind::indext	getindex() const { return value; }
	point				getposition() const { return i2h(value); }
	void				setposition(point v) { value = h2i(v); }
	void				setposition(pathfind::indext v) { value = v; }
};
class decoration : public indexable {
	const tilei*		parent;
public:
	explicit operator bool() const { return parent != 0; }
	static decoration*	add(const char* id, point position);
	bool				is(tile_s v) const { return (parent - bsdata<tilei>::elements) == v; }
	void				updateui() const;
};
struct decorationa : adat<decoration*> {
	void				range(int v);
	void				select();
};
class creaturei : public indexable {
	const void*			parent;
	char				level, hits;
	statef				state;
public:
	static creaturei*	active;
	explicit operator bool() const { return parent != 0; }
	void				activate();
	static creaturei*	add(const char* id, point position, bool elite = false);
	void				addcoins(int value);
	void				addexperience(int value);
	void				apply(variants source);
	void				apply(action_s type);
	void				apply(state_s type);
	void				apply(target_s type);
	void				attack(creaturei& enemy, int bonus, int pierce = 0, int advantage = 0);
	void				choosecards();
	creaturei*			chooseenemy() const;
	void				chooseinitiative();
	creaturei*			choosenearest(bool hostile, int range) const;
	void				clear();
	void				damage(int v);
	static int			get(modifier_s i);
	int					getactive(action_s id) const;
	int					getactive(action_s id, variant* p, variant* pe) const;
	int					getcoins() const;
	combatdeck&			getcombatdeck() const;
	void				getdefence(int& attack, int& retaliate);
	int					getexperience() const;
	const char*			getid() const;
	int					getinitiative(int index = 0) const;
	int					gethp() const { return hits; }
	int					getmaximumhp() const;
	static variant*		getmodifiers(variant* p, variant* pe, char* modifiers);
	static void			getmodifiers(stringbuilder& sb);
	const summoni*		getmonster() const;
	pathfind::indext	getmovetarget() const;
	creaturei*			getnearestenemy() const;
	int					getongoing(action_s v) const;
	playeri*			getplayer() const;
	void				heal(int v);
	bool				is(state_s v) const { return state.is(v); }
	bool				iscomputer() const;
	bool				isplayer() const;
	void				kill();
	void				loot(int bonus);
	void				move(int bonus);
	bool				moveto(pathfind::indext index, int bonus, int range);
	void				paint() const;
	void				play();
	bool				pull(pathfind::indext from, int bonus);
	bool				push(pathfind::indext from, int bonus);
	void				set(state_s v) { return state.set(v); }
	void				updateui() const;
	void				useshield(int& attack);
};
struct creaturea : adat<creaturei*> {
	static const creaturea* last;
	creaturei*			choose(const char* title) const;
	void				match(state_s v, bool keep);
	void				range(int v);
	void				select();
	void				sort();
	void				sortbymove();
};
namespace draw {
void					waitall();
}
struct gamei : public location {
	char				elements[Dark + 1];
	combatdeck			combat;
	creaturea			targets;
	int					dungeon_level;
	static void			checkinitiative();
	static void			choosecards();
	static void			dropcoin(point hex);
	static void			focusing(point pt);
	static void			initialize();
	static duration_s	getduration(variants source);
	static int			getrounds(variants source);
	static void			playmoves();
	static void			playround();
	void				set(element_s v) { elements[v] = 2; }
	static void			updateui(void* parent, point position);
};
namespace pathfind {
void					blockwalls();
}
extern gamei			game;