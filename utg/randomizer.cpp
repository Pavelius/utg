#include "bsreq.h"
#include "randomizer.h"
#include "script.h"

BSMETA(randomizeri) = {
	BSREQ(id),
	BSREQ(chance),
	{}};
BSDATAC(randomizeri, 512)

static int getcounter(variant v) {
	if(v.counter < 1)
		return 1;
	return v.counter;
}

int randomizeri::total(const variants& elements) {
	auto result = 0;
	for(auto& e : elements)
		result += getcounter(e);
	return result;
}

variant randomizeri::random(const variants& elements, int range, int bonus) {
	variant lr = variant();
	auto summary = total(elements);
	if(!range)
		range = summary;
	if(summary) {
		auto result = bonus + (rand() % summary);
		for(auto& e : elements) {
			auto n = getcounter(e);
			lr = e; lr.counter = 0;
			if(result < n)
				break;
			result -= n;
		}
	}
	return lr;
}

variant single(variant v) {
	while(v.iskind<randomizeri>())
		v = bsdata<randomizeri>::elements[v.value].random();
	return v;
}

variant single(variant v, int bonus) {
	while(v.iskind<randomizeri>())
		v = bsdata<randomizeri>::elements[v.value].random(bonus);
	return v;
}

template<> void fnscript<randomizeri>(int index, int bonus) {
	variant v = single(bsdata<randomizeri>::elements + index);
	script_run(v);
}