#include "hero.h"
#include "stringbuilder.h"

template<> void ftinfo<hero>(const void* object, stringbuilder& sb) {
	auto p = (hero*)object;
	sb.add(p->getname());
}

void hero::getproperty(const void* object, variant v, stringbuilder& sb) {
	auto p = (hero*)object;
	if(v.iskind<skilli>()) {
		auto value = p->getskill((skill_s)v.value);
		if(!value)
			return;
		sb.add("%1i", value);
		auto pass = p->skills_pass[v.value];
		auto fail = p->skills_fail[v.value];
		sb.adds("[~");
		if(pass)
			sb.adds("%Success(%1i)", pass);
		if(fail)
			sb.adds("%Fail(%1i)", fail);
		sb.adds("]");
	}
}