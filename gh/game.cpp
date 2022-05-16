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

int gamei::getrounds(variants source) {
	if(source) {
		auto v = source.begin();
		if(v->iskind<durationi>() && v->value==Use)
			return v->counter;
	}
	return 0;
}