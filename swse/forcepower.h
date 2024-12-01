#pragma once
#include "nameable.h"

enum forcetypen : unsigned char {
	CommonForce, Telekinetic, DarkSide, LightSide,
};
struct forcetypei : nameable {
};
struct forcepower : nameable {
	forcetypen type;
};
struct forceable {
	char forcepowers[32];
};