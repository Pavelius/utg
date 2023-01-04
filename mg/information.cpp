#include "hero.h"

template<> void ftstatus<hero>(const void* object, stringbuilder& sb) {
	auto p = (hero*)object;
	sb.add(p->getname());
}

void hero::getproperty(const void* object, variant v, stringbuilder& sb) {
	auto p = (hero*)object;
	if(v.iskind<skilli>())
		sb.add("%1i", p->getskill((skill_s)v.value));
}