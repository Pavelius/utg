#include "ability.h"
#include "answers.h"
#include "condition.h"
#include "crt.h"
#include "deck.h"
#include "flagable.h"
#include "gender.h"
#include "menu.h"
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
	EnemyAttackYouInsteadNearestAlly, GainExpForRetaliate, GainExpForTarget,
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
	Shield, Retaliate,
	Move, Attack, Push, Pull, Heal, DisarmTrap, Loot,
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
typedef short unsigned indext;
typedef flagable<2> statef;
typedef flagable<1 + TargetEnemyMoveThrought / 8> featf;
typedef flagable<1> elementf;
typedef flagable<4>	playerf;
const indext Blocked = 0xFFFF;
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
struct playeri {
	const char*			id;
	gender_s			gender;
	char				level, exp, coins;
	variant				cards[2];
	short				health[10];
	deck				combat, hand, discard, lost;
};
struct playercardi {
	const char*			id;
	char				level;
	playeri*			owner;
	int					initiative;
	variants			upper, lower;
	void				getinfo(stringbuilder& sb) const;
};
struct monstercardi {
	const char*			id;
	int					initiative;
	variants			abilities;
	void				getinfo(stringbuilder& sb) const;
};
struct combatcardi {
	const char*			id;
	variant				owner;
	char				count, bonus;
	variants			feats;
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
struct monsteri {
	const char*			id;
	const char*			abilities;
	deck				abilities_deck;
	abilityi			normal[8], elite[8];
	void				buildcombatdeck();
	const abilityi&		get(int level, bool tought) const;
	int					getinitiative() const;
	void				getinfo(stringbuilder& sb) const;
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
	action				actions[6];
	void				clear();
	static bool			read(const char* url, array& source);
};
struct scenariotilei {
	const tilei*		type;
	point				position;
	bool				inverse;
};
struct scenarioi {
	const char*			id;
	scenariotilei		tiles[16];
};