#include "nameable.h"
#include "flagable.h"

#pragma once

enum skill_s : unsigned char {
	Acrobatics, Climb, Deception, Endurance, GatherInformation,
	Initiative, Jump, Mechanics, Perception, Persuasion,
	Pilot, Ride, Stealth, Survival, Swim,
	TreatInjury, UseComputer, UseTheForce,
	Bureaucracy, GalacticLore, LifeSciences, PhysicalSciences, SocialSciences,
	Tactics, Technology
};
typedef flagable<4> skilla;
struct skillable {
	char		skills[Technology + 1];
};
struct skilli : nameable {
	char		ability;
};
