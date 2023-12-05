#include "crt.h"
#include "flagable.h"
#include "nameable.h"

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
	Persona, Fate, Success, Bonus
};
typedef flagable<(Weaver + 7) / 8> skilla;
typedef char skillds[Bonus + 1];
struct skilli : nameable {
	skilla		help;
};
struct skillable {
	skillds		skills, skills_fail, skills_pass;
	void		addskill(skill_s v, int i) { setskill(v, getskill(v) + i); }
	int			getskill(skill_s v) const { return skills[v]; }
	void		setskill(skill_s v, int i) { skills[v] = i; }
	void		setskills(const skillds p) { memcpy(skills, p, sizeof(skills)); }
};