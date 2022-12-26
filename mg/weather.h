#include "condition.h"
#include "skill.h"

#pragma once

enum season_s : unsigned char {
	Spring, Summer, Autum, Winter,
};
struct weatheri {
	const char*	id;
	char		chance;
	season_s	season;
	season_s	season_link;
	conditiona	conditions;
	bool		weather_factor_for_outdoor;
	char		obstacle_for_tired;
	char		obstacle_for_sick;
	skilla		skills;
	constexpr bool nonseason() const { return season != season_link; }
};
struct seasoni {
	const char* id;
	char		obstacle;
	const weatheri* epic;
};