#include "flagable.h"

#pragma once

enum skill_s : unsigned char {
	Nature, Will, Health, Resources, Circles,
	Administrator, Apiarist, Archivist, Armorer, Baker,
	Boatcrafter, Brewer, Carpenter, Cartographer, Cook,
	Deceiver, Fighter, Glazier, Haggler, Harvester,
	Healer, Hunter, Insectrist, Instructor, Laborer,
	Loremouse, Militarist, Miller, Orator, Pathfinder,
	Persuader, Potter, Scientist, Scout, Smith,
	Stonemason, Survivalist, WeatherWatcher, Weaver,
	FirstSkill = Administrator, LastSkill = Weaver,
};
typedef flagable<(LastSkill + 7) / 8> skilla;
typedef char skillds[LastSkill + 1];
struct skilli {
	const char* id;
	skilla		help;
};