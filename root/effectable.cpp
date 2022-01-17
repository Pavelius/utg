#include "main.h"

static variant getobject(variant v) {
	if(v.type == Bonus)
		return bsdata<bonusi>::elements[v.value].object;
	return v;
}

static int getvalue(variant v) {
	if(v.type == Bonus)
		return bsdata<bonusi>::elements[v.value].value;
	return 1;
}

static int getparam(variant v) {
	if(v.type == Bonus)
		return bsdata<bonusi>::elements[v.value].param;
	return 0;
}

void effectable::apply(variant object) {
	auto v = getobject(object);
	auto c = getvalue(object);
	auto m = getparam(object);
	harmable* ph;
	switch(v.type) {
	case Resource:
		ph = &inflict;
		switch(m) {
		case 1: ph = &suffer; break;
		case 2: ph = &group; break;
		default: ph = &inflict; break;
		}
		if(v.value == Harm)
			v.value = ph->getdefault();
		if(v.value < sizeof(ph->harm) / sizeof(ph->harm[0]))
			ph->harm[v.value] += c;
		break;
	}
}

void effectable::clear() {
	memset(this, 0, sizeof(*this));
}