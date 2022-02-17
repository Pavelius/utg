#include "harmable.h"

void harmable::clear() {
	memset(this, 0, sizeof(*this));
}

int harmable::getdistinct() const {
	auto result = 0;
	for(auto v : harm) {
		if(v)
			result++;
	}
	return result;
}

int harmable::getdistinct(harma source) const {
	auto n = 0;
	for(auto v : source) {
		if(harm[v])
			n++;
	}
	return n;
}

void harmable::getinfo(stringbuilder& sb, harma source) const {
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