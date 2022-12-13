#include "harmable.h"

BSDATA(harmi) {
	{"Injury"},
	{"Exhaustion"},
	{"Wear"},
	{"Depletion"},
	{"Morale"},
	{"Value"},
};
assert_enum(harmi, Value)

void harmable::clear() {
	memset(this, 0, sizeof(*this));
}

harm_s harmable::getdefault() const {
	for(auto& v : harm) {
		if(v)
			return harm_s(&v - harm);
	}
	return harm_s(0);
}

int harmable::getdistinct() const {
	auto result = 0;
	for(auto v : harm) {
		if(v)
			result++;
	}
	return result;
}

int harmable::getdistinct(const harma& source) const {
	auto n = 0;
	for(auto v : source) {
		if(harm[v])
			n++;
	}
	return n;
}

void harmable::getinfo(stringbuilder& sb, const harma& source) const {
	auto n = getdistinct(source);
	auto pb = sb.get(); pb[0] = 0;
	for(auto vi : source) {
		auto v = harm[vi];
		if(v <= 0)
			continue;
		auto pn = bsdata<harmi>::elements[vi].id;
		if(pb[0]) {
			if(n == 1)
				sb.adds("%-And");
			else
				sb.add(",");
		}
		if(v == 1)
			sb.adds("%-1", getnm(pn));
		else
			sb.adds("[%2i] %1", sb.getbycount(pn, v), v);
		n--;
	}
}

void harmable::getinfo(stringbuilder& sb) const {
	auto start = sb.get(); start[0] = 0;
	for(auto i = harm_s(0); i <= Value; i = harm_s(i + 1)) {
		auto v = harm[i];
		if(!v)
			continue;
		sb.adds("[%2i] %1", getnm(bsdata<harmi>::elements[i].id), v);
	}
	if(start[0])
		sb.add(".");
}