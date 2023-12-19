#include "crt.h"
#include "interval.h"

int interval::roll() const {
	return max ? min + rand() % (max - min) : min;
}