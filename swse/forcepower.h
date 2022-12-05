#pragma once
#include "nameable.h"

enum forcetype_s : unsigned char {
	CommonForce, Telekinetic, DarkSide, LightSide,
};
struct forcetypei : nameable {

};
struct forcepower : nameable {
};
struct forceable {
	char forcepowers[32];
};
