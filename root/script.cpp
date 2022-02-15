#include "answers.h"
#include "harmable.h"
#include "script.h"
#include "variant.h"

static int bad_moves, good_moves, bonus_forward;
static int last_value, last_index;
static harmable* last_harm;
static harmable inflict, suffer, inflict_group;
static bool need_stop;

static void apply_effect(const variants& tags) {
	need_stop = false;
	last_harm = &inflict;
	last_value = 0;
	for(auto v : tags) {
		if(need_stop)
			break;
		v.apply();
	}
}

void variant::apply() const {
	if(iskind<harmi>()) {
		last_index = value;
		last_value = counter;
		if(last_harm) {
			if((size_t)last_index < sizeof(last_harm->harm) / sizeof(last_harm->harm[0])) {
				last_value += last_harm->harm[last_index];
				last_harm->harm[last_index] = last_value;
			}
		}
	} else if(iskind<scripti>())
		bsdata<scripti>::elements[value].apply(counter);
}

static void if_equal(int bonus, int param) {
	if(last_value != bonus)
		need_stop = true;
}

static void if_not_equal(int bonus, int param) {
	if(last_value == bonus)
		need_stop = true;
}

static void set_harm(int bonus, int param) {
	switch(param) {
	case 1: last_harm = &suffer; break;
	default: last_harm = &inflict; break;
	}
}

static void you_inflict(int bonus, int param) {
	set_harm(0, 0);
}

static void apply_harm(int bonus, int param) {
	set_harm(0, 0);
}

BSDATA(scripti) = {
	{"IfEqual", if_equal},
	{"IfNotEqual", if_not_equal},
	{"Inflict", set_harm, 0},
	{"InflictHarm", apply_harm, 0},
	{"Suffer", set_harm, 1},
	{"SufferHarm", apply_harm, 1},
};
BSDATAF(scripti)

void scripti::apply(int bonus) const {
	if(proc)
		proc(bonus, param);
}