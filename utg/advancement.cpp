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

void* advancement::choose() const {
	answers an;
	if(elements) {
		for(auto v : elements)
			an.add(v.getpointer(), v.getname());
	}
	return an.choose(getnm(id), 0, logs::interactive, logs::url, 1, logs::header);
}

void advancement::apply(void* object) const {
	auto pm = varianti::getmetadata(object);
	if(!pm)
		return;
	auto pv = choose();
	pm->set(object, result, pv);
}