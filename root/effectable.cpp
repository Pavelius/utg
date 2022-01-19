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

harmable& effectable::getharm(int m) {
	switch(m) {
	case 1: return suffer;
	case 2: return group;
	default: return inflict;
	}
}

void effectable::apply(variant object) {
	auto v = getobject(object);
	auto c = getvalue(object);
	auto m = getparam(object);
	switch(v.type) {
	case Special:
		switch(v.value) {
		case Harm:
			apply(getharm(m).getdefault());
			break;
		case Ignore:
			getharm(m).clear();
			break;
		}
		break;
	case Resource:
		if(v.value < sizeof(harmable::harm) / sizeof(harmable::harm[0]))
			getharm(m).harm[v.value] += c;
		break;
	}
}

void effectable::clear() {
	memset(this, 0, sizeof(*this));
}