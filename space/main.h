#include "ability.h"
#include "answers.h"
#include "gender.h"
#include "message.h"
#include "tag.h"
#include "timeable.h"
#include "utg.h"

#pragma once

enum tag_s : unsigned char {
	Intimate, Close, Far,
};
enum ability_s : unsigned char {
	Assault, Comunication, Exploration, Medicine, Hunting, Research,
};
enum state_s : unsigned char {
	Crew, Discontent, Supply, Machinery,
	Inside, Problem, Success, Advantage,
};
typedef flagable<8> taga;
struct statei {
	const char*		id;
};
class abilitya {
	char			abilities[Research + 1];
	char			states[Advantage + 1];
public:
	void			add(state_s v, int i) { states[v] += i; }
	int				get(ability_s v) const { return abilities[v]; }
	int				get(state_s v) const { return states[v]; }
	void			set(ability_s v, int i) { abilities[v] = i; }
	void			set(state_s v, int i) { states[v] = i; }
};
struct missioni : abilitya {
	void			apply(variant v);
	void			apply(variants source);
	void			beforemove();
	void			clear();
};
struct gamei : timeable, missioni {
};
extern gamei		game;
extern ability_s	last_ability;
void				actv(const char* format, const char* format_param);
inline void			act(const char* format, ...) { actv(format, xva_start(format)); }
void				information(const char* format, ...);