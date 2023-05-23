#include "crt.h"
#include "statable.h"

#define ISU(F, V) ((F & FG(V))!=0)

void statable::copy(const statable& v) {
	memcpy(this, &v, sizeof(*this));
}

bool statable::isimmunity(int material) const {
	return ISU(immunity, material);
}

bool statable::isresistance(int material) const {
	return ISU(resistance, material);
}

bool statable::isvulnerable(int material) const {
	return ISU(vulnerable, material);
}