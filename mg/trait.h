#include "skill.h"

#pragma once

enum trait_s : unsigned char {
	Bigpaw, Bitter, Bodyguard, Bold, Brave,
	Calm, Clever, Compassionate, Cunning, Curious,
	DeepEar, Defender, Determined, Driven, EarlyRiser,
	Extrovert, Fat, Fearful, Fearless, Fiery,
	Generous, Graceful, GuardsHonor, Innocent, Jaded,
	Leader, Longtail, Lost, NaturalBearings, Nimble,
	Nocturnal, Oldfur, QuickWitted, Quiet, Scarred,
	SharpEyed, Sharptooth, Short, Skeptical, Skinny,
	Stoic, Stubborn, Suspicious, Tall, Thoughtful,
	Tough, WeatherSense, WiseTrait, WolfsSnout, Young,
	Alert, HardWorker, Independent, OpenMinded, SteadyPaw, Inquisitive, Rational,
	FirstTraits = Bigpaw, LastTraits = Rational,
};
typedef char traita[LastTraits+1];
struct traiti : nameable {
	skilla		advantages, disadvantages;
};
class traitable {
	traita		traits, used_traits;
public:
	bool		isreadytrait(trait_s v) const { return used_traits[v] < traits[v]; }
	int			gettrait(trait_s v) const { return traits[v]; }
	void		settrait(trait_s v, int i) { traits[v] = i; }
	void		usetrait(trait_s v) { used_traits[v]++; }
};