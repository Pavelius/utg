#include "main.h"

gamei game;

duration_s gamei::getduration(variants source) {
	if(source) {
		auto v = source.begin();
		if(v->iskind<durationi>())
			return (duration_s)v->value;
	}
	return Instant;
}