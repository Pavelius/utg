#include "nameable.h"
#include "feat.h"

#pragma once

enum skilln : unsigned char;
//enum skill_s : unsigned char {
//	Acrobatics, Climb, Deception, Endurance, GatherInformation,
//	Initiative, Jump, Mechanics, Perception, Persuasion,
//	Pilot, Ride, Stealth, Survival, Swim,
//	TreatInjury, UseComputer, UseTheForce,
//	Bureaucracy, GalacticLore, LifeSciences, PhysicalSciences, SocialSciences,
//	Tactics, Technology
//};
typedef flag32 skillf;
struct skillable {
	skillf	skills_train;
	bool	istrain(skilln v) const { return skills_train.is(v); }
};
struct skilli : nameable {
	char	ability;
	featf	reroll;
};
extern skilln last_skill;