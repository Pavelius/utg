#include "interval.h"
#include "math.h"
#include "rand.h"

int interval::roll() const {
	return max ? min + rand() % (max - min) : min;
}

interval interval::mul(int k) const {
	return {(unsigned short)(min * k), (unsigned short)(max * k)};
}

interval interval::add(int k, int m) const {
	short unsigned n1 = imax(m, min + k);
	short unsigned n2 = imax(m, max + k);
	return {n1, n2};
}

int rollint(int min, int max) {
	return max ? min + rand() % (max - min) : min;
}