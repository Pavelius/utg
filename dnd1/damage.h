#include "feat.h"
#include "nameable.h"

#pragma once

enum damage_s : unsigned char {
	Iron, Wood, Fire, Cold,
};
struct damagei : nameable {
	featn	immunity, resistance;
};
