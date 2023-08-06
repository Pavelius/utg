#include "actioncard.h"
#include "crt.h"
#include "decka.h"
#include "pathfind.h"
#include "point.h"
#include "playera.h"
#include "script.h"
#include "strategy.h"
#include "tech.h"
#include "variant.h"
#include "unit.h"

const int hms = 8;

enum racef_s : unsigned char {
	Assimilate, Fragile, Unrelenting
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
struct troop : entity {
	flagable<1>		flags;
	static troop*	create(const char* id, playeri* player);
	static troop*	create(const uniti* unit, playeri* player, entity* location);
	void			clear() { memset(this, 0, sizeof(*this)); }
	void			getinfo(stringbuilder& sb) const;
	void			hit();
	void			paint(unsigned flags) const;
	void			produce(const uniti* unit) const;
	static void		updateui();
};
extern troop* last_troop;
struct agendai : nameable {
	variants		target;
	variants		yes, no;
};
extern decka actioncards;
class analize {
	pathfind::indext data[hms * hms];
public:
	void			clear();
	int				getweight(pathfind::indext i) const;
	void			setweight(pathfind::indext i, int v);
};
struct unitupgrade : uniti, requirement {
};
struct gamei {
	playeri*		speaker;
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
};
extern gamei game;

inline point i2h(pathfind::indext i) { return {(short)(i % hms), (short)(i / hms)}; }
inline pathfind::indext	h2i(point v) { return v.y * hms + v.x; }