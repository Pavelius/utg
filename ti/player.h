#include "nameable.h"
#include "indicator.h"
#include "tech.h"
#include "variant.h"

#pragma once

struct planeti;
struct strategyi;
struct systemi;
struct uniti;

enum racef_s : unsigned char;

struct playeri : nameable {
	char			indicators[VictoryPoints + 1];
	techa			tech, tech_used;
	flagable<4>		race;
	char			commodities;
	strategyi*		strategy;
	variants		troops, startup;
	bool			use_strategy;
	bool			pass_action_phase;
	void			actv(const char* format, const char* format_param) const;
	void			add(indicator_s v, int i);
	void			apply(const variants& source);
	bool			ask(const char* header_id, const char* format, ...) const;
	bool			askv(const char* header_id, const char* format, const char* format_param) const;
	void			assign(variants source);
	bool			canbuild(const uniti* player) const;
	void			event(const char* id);
	bool			is(tech_s v) const { return tech.is(v); }
	bool			is(racef_s v) const { return race.is(v); }
	bool			ishuman() const;
	int				get(indicator_s v) const { return indicators[v]; }
	void			getadvance(requirement& result) const;
	int				getactioncards() const;
	systemi*		gethome() const;
	int				getindex() const;
	int				getinitiative() const;
	void			getinfo(stringbuilder& sb) const;
	int				getplanetsummary(indicator_s v) const;
	int				getsummary(const uniti* type) const;
	int				gettechs() const;
	const uniti*	getunit(int index) const;
	void			sayv(const char* format, const char* format_param) const;
	void			sayspeech(const char* id) const;
	void			set(indicator_s v, int i) { indicators[v] = i; }
	void			setcontrol(planeti* p);
};
extern playeri*	player;
extern playeri* human_player;

bool reaction(const char* id, const playeri* need_player, const playeri* exclude_player, ...);
