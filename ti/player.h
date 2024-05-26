#include "color.h"
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
	color			fore;
	techa			tech, tech_used;
	flagable<4>		race;
	char			commodities;
	strategyi*		strategy;
	variants		troops, startup;
	bool			use_strategy;
	bool			pass_action_phase;
	void			activate();
	void			actv(const char* format, const char* format_param) const;
	void			add(indicator_s v, int i);
	void			apply(const variants& source);
	bool			ask(const char* header_id, const char* format, ...) const;
	bool			askv(const char* header_id, const char* format, const char* format_param) const;
	void			assign(variants source);
	bool			canbuild(const uniti* player) const;
	void*			choose(const char* format);
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
extern playeri* speaker;
extern playeri* human_player;

extern int finale_score;

int	getrate(indicator_s need, indicator_s currency, int count);

bool reaction(const char* id, const playeri* need_player, const playeri* exclude_player, ...);

void initialize_game();
void limit_by_capacity();
void play_game();
void players_sort_by_initiative();
void prepare_game();
void prepare_game_ui();
void update_control();
void update_ui();
