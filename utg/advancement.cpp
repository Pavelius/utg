#include "advancement.h"
#include "answers.h"
#include "bsreq.h"
#include "utg.h"

BSMETA(advancement) = {
	BSREQ(type),
	BSREQ(id),
	BSREQ(result),
	BSREQ(choose),
	BSREQ(elements),
	{}};
BSDATAC(advancement, 256)

static void def_add(answers& an, const void* object, variant v) {
	an.add(v.getpointer(), v.getname());
}

static const array* getarray(const void* object, const char* id) {
	auto pm = bsdata<varianti>::find(id);
	if(!pm)
		return 0;
	return pm->source;
}

static adat<void*, 32> applied_answers;

void* advancement::choosevalue(const void* object, fnadd padd) const {
	answers an;
	if(!padd)
		padd = def_add;
	if(elements) {
		for(auto v : elements) {
			if(applied_answers.is(v.getpointer()))
				continue;
			padd(an, object, v);
		}
	} else {
		auto ps = getarray(object, result);
		if(!ps)
			return 0;
		auto pe = ps->end();
		for(auto p = ps->begin(); p < pe; p += ps->size) {
			if(applied_answers.is(p))
				continue;
			padd(an, object, p);
		}
	}
	auto result = an.choose(getnm(id), 0);
	applied_answers.add(result);
	return result;
}

static void apply_result(void* object, advancement::fnset pset, const char* result, void* data, int count) {
	if(pset && pset(object, result, data, count))
		return;
	auto pm = varianti::find(object);
	if(!pm)
		return;
	auto pmv = varianti::find(data);
	if(!pmv)
		return;
	auto pr = pm->metadata->find(result);
	if(!pr)
		return;
	auto index = pmv->source->indexof(data);
	pr->set(pr->ptr(object), index);
}

static void choose_value(const advancement* this_value, void* object, advancement::fnadd padd, advancement::fnset pset) {
	auto pv = this_value->choosevalue(object, padd);
	apply_result(object, pset, this_value->result, pv, 1);
}

void advancement::apply(void* object, fnadd padd, fnset pset) const {
	applied_answers.clear();
	if(choose > 0) {
		auto need_choose = choose;
		while(need_choose--)
			choose_value(this, object, padd, pset);
	} else {
		for(auto v : elements)
			apply_result(object, pset, result, v.getpointer(), v.counter);
	}
}