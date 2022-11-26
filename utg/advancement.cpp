#include "advancement.h"
#include "answers.h"
#include "bsreq.h"
#include "utg.h"

BSMETA(advancement) = {
	BSREQ(type),
	BSREQ(id),
	BSREQ(result), BSREQ(mask),
	BSREQ(elements), BSREQ(conditions),
	{}};
BSDATAC(advancement, 256)

static void def_add(answers& an, const void* object, variant v) {
	an.add(v.getpointer(), v.getname());
}

void* advancement::choose(const void* object, fnadd padd) const {
	answers an;
	if(!padd)
		padd = def_add;
	if(elements) {
		for(auto v : elements)
			padd(an, object, v);
	} else {
		auto ps = varianti::getarray(object, result);
		if(!ps)
			return 0;
		auto pe = ps->end();
		for(auto p = ps->begin(); p < pe; p += ps->size)
			padd(an, object, p);
	}
	return an.choose(getnm(id), 0);
}

void advancement::apply(void* object, fnadd padd, fnset pset) const {
	auto pv = choose(object, padd);
	if(pset && pset(object, result, pv))
		return;
	auto pm = varianti::find(object);
	if(!pm)
		return;
	pm->set(object, result, pv);
}