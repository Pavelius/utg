#include "nameable.h"

#pragma once

enum combat_s : unsigned char {
	ScoutingPhase, LongRange, MediumRange, CloseRange, FinalPhase,
};
struct combati : nameable {
};