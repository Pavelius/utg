#include "answers.h"
#include "condition.h"
#include "harmable.h"
#include "quest.h"
#include "script.h"
#include "tag.h"
#include "variant.h"

static int bad_moves, good_moves, bonus_forward;
static int last_value, last_index;
static const char* need_prompt;
static harmable* last_harm;
static harmable inflict, suffer;
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

static bool allow_prompt(const variants& tags) {
	for(auto v : tags) {
		conditioni* pc = v;
		if(pc && !pc->proc(v.counter, pc->param))
			return false;
	}
	return true;
}

static const quest* find_prompt(int index) {
	for(auto& e : bsdata<quest>()) {
		if(e.index != index)
			continue;
		if(e.isanswer())
			continue;
		if(!allow_prompt(e.tags))
			continue;
		return &e;
	}
	return 0;
}

static const quest* find_forward(int index) {
	auto pe = bsdata<quest>::end();
	for(auto pb = quest::last; pb < pe; pb++) {
		if((pb->index >= AnswersRoll && pb->index < AnswersForward)
			|| (pb->index >= AnswersForward + 100))
			break;
		if(pb->index != index)
			continue;
		if(pb->isanswer())
			continue;
		if(!allow_prompt(pb->tags))
			continue;
		return pb;
	}
	return 0;
}

static void clear_log() {
	if(answers::console)
		answers::console->clear();
}

static void add_prompt(const quest* p) {
	if(!p)
		return;
	auto id = getbsi(p);
	auto pi = getstring(id, quest::prop_header);
	if(pi)
		answers::header = pi;
	pi = getstring(id, quest::prop_image);
	if(pi)
		answers::resid = pi;
	if(answers::console && p->text)
		actv(*answers::console, p->text, 0, '\n');
}

static void add_answer(answers& an, const quest* p) {
	an.add(p, p->text);
}

static const quest* choose_answer(const quest* ph) {
	answers an;
	auto index = ph->index;
	auto pe = bsdata<quest>::end();
	for(auto p = ph + 1; p < pe; p++) {
		if(p->index != index)
			break;
		if(p->next < AnswersRoll)
			continue;
		add_answer(an, p);
	}
	if(!an)
		return 0;
	auto p = (const quest*)an.choose(0);
	need_prompt = 0;
	if(p && p->next > 0) {
		if(p->next >= AnswersForward && p->next < AnswersForward + 100)
			return find_forward(p->next);
		return find_prompt(p->next);
	}
	return 0;
}

void quest::run(int index) {
	last = find_prompt(index);
	while(last) {
		clear_log();
		need_prompt = "Continue";
		add_prompt(last);
		last = choose_answer(last);
		if(need_prompt)
			draw::pause(need_prompt);
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
	else if(iskind<tagi>()) {
		// TODO: add tag or may be nothing?
	} else
		draw::warning(getnm("NotSupportType"), bsdata<varianti>::elements[type].id, value);
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