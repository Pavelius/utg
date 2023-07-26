#include "nameable.h"

#pragma once

enum goal_s : unsigned char {
	CornerTheMarket, DevelopWeaponry, DiversifyResearch, ErectAMonument,
	ExpandBorders, FoundResearchOutposts, IntimidateCouncil, LeadFromTheFront,
	NegotiateTradeRoutes, SwayTheCouncil,
	CentralizeGalacticTrade, ConquerTheWeak, FormGalacticBrainTrust, FoundAGoldenAge,
	GalvanizeThePeople, ManipulateGalacticLaw, MasterTheSciences, RevolutionizeWarfare,
	SubdueTheGalaxy, UnifyTheColonies
};

struct goali : nameable {
	typedef int(*fnget)();
	int			score, value;
	int			param;
};

int getgoal(goal_s v);

bool need_spend(goal_s v);
