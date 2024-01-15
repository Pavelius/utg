#include "ability.h"
#include "answers.h"
#include "harmable.h"
#include "modifier.h"
#include "npc.h"
#include "quest.h"
#include "script.h"
#include "vagabond.h"

static void fix_chance(const char* format, const char* name, int value) {
	if(value > 0)
		information(format, name, value);
	else
		warning(format, name, value);
}

static void fix_chancer(const char* format, const char* name, int value) {
	if(value <= 0)
		information(format, name, value);
	else
		warning(format, name, value);
}

template<> void fnscript<abilityi>(int index, int bonus) {
	player->harm[index] += bonus;
}

template<> void fnscript<harmi>(int index, int bonus) {
	switch(modifier) {
	case Inflict:
		fix_chance(getnm("InflictHarm"), getnm(bsdata<harmi>::elements[index].id), bonus);
		inflict.harm[index] += bonus;
		break;
	case Suffer:
		fix_chancer(getnm("SufferHarm"), getnm(bsdata<harmi>::elements[index].id), bonus);
		suffer.harm[index] += bonus;
		break;
	default: break;
	}
}

static void add_new(const char* format) {
	pause();
	answers::console->add(format);
}

static void change_range(int bonus) {
}

static void inflict_harm(int bonus) {
	inflict.clear();
	if(player)
		inflict = player->inflict;
}

static void suffer_harm(int bonus) {
	suffer.clear();
	if(opponent)
		suffer = opponent->inflict;
}

static void default_harm(int bonus) {
	auto v = Injury;
	switch(modifier) {
	case Inflict: v = inflict.getdefault(); break;
	case Suffer: v = suffer.getdefault(); break;
	default: break;
	}
	fnscript<harmi>(v, bonus);
}

static void choose_step(int bonus) {
	movei::choose_count = bonus;
}

static void apply_harm(int bonus) {
}

BSDATA(script) = {
	{"ApplyHarm", apply_harm},
	{"ChangeRange", change_range},
	{"Choose", choose_step},
	{"Harm", default_harm},
	{"InflictHarm", inflict_harm},
	{"SufferHarm", suffer_harm},
};
BSDATAF(script)