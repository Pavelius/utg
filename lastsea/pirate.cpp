#include "main.h"

void pirate::clear() {
	memset(this, 0, sizeof(*this));
	historyable::clear();
}

const char* pirate::getavatarst(const void* object) {
	auto p = (pirate*)object;
	return p->avatar;
}

void pirate::generate() {
	chooseclass();
}

bool pirate::match(variant v) const {
	switch(v.type) {
	case Class: return classid == v.value;
	case Gender: return getgender() == v.value;
	default: return false;
	}
}