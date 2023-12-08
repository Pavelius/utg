#include "nameable.h"
#include "feat.h"

#pragma once

enum skill_s : unsigned char;
//enum skill_s : unsigned char {
//	Acrobatics, Climb, Deception, Endurance, GatherInformation,
//	Initiative, Jump, Mechanics, Perception, Persuasion,
//	Pilot, Ride, Stealth, Survival, Swim,
//	TreatInjury, UseComputer, UseTheForce,
//	Bureaucracy, GalacticLore, LifeSciences, PhysicalSciences, SocialSciences,
//	Tactics, Technology
//};
typedef flagable<4> skillf;
struct skillable {
	skillf	skills_train;
};
struct skilli : nameable {
	char	ability;
	featf	reroll;
};
extern skill_s last_skill;