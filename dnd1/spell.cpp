#include "main.h"

BSDATA(spelli) = {
	{"CureLightWound", {1}, Instant, CasterOrCreatureTouched, {1, 6, 1}},
	{"DetectEvil", {1}, Hour, Range120},
	{"DetectMagic", {1, 1}, Turn2, Range60},
	{"Light", {1, 1}, Hour2, Range120},
	{"ProtectionFromEvil", {1, 1}, Hour2, Caster},
	{"PurifyFoodAndWater", {1}, Permanent, Range10},
	{"RemoveFear", {1}, Instant, CasterOrCreatureTouched},
	{"ResistCold", {1}, Hour, Range30},
	{"CharmPerson", {0, 1}, Instant, Range120},
	{"FloatingDisc", {0, 1}, Hour, Range5},
	{"HoldPortal", {0, 1}, Turn2d6, Range10},
	{"MagicMissile", {0, 1}, Instant, Range150, {1, 6, 1}},
	{"ReadLanguages", {0, 1}, Turn2, Caster},
	{"ReadMagic", {0, 1}, Turn, Caster},
	{"Shield", {0, 1}, Turn2, Caster},
	{"Sleep", {0, 1}, Turn4d4, Range240},
	{"Ventriloquism", {0, 1}, Turn2, Range60},
	{"ContinualLight", {0, 2}, Hour2, Caster},
	{"DetectInvisibility", {0, 2}, Instant, AllCreatures, {}, {Invisibility}},
	{"ESP", {0, 2}, Hour2, Caster},
	{"Invisibility", {0, 2}, Permanent, CasterOrCreatureTouched},
	{"Knock", {0, 2}, Instant, Caster},
	{"Levitate", {0, 2}, Hour, Caster},
	{"MirrorImages", {0, 2}, Hour, Caster},
	{"PhantasmalForce", {0, 2}, Hour, Range240},
	{"Web", {0, 2}, Hour8, Caster},
	{"WizardLock", {0, 2}, Permanent, Caster},
	{"ShrinkSize", {}, Turn, CasterOrCreatureTouched},
	{"GrowthSize", {}, Turn, CasterOrCreatureTouched},
	{"GaseousForm", {}, Turn, CasterOrCreatureTouched},
	{"DeathPoison", {}, Turn, EnemyCreatureTouched},
};
assert_enum(spelli, DeathPoison)

static int getduration(duration_s d, int level) {
	auto r1 = bsdata<durationi>::elements[d].from;
	auto r2 = bsdata<durationi>::elements[d].to;
	if(r2 == 0)
		return r1;
	return xrand(r1, r2);
}

bool creature::apply(spell_s id, int level, bool run) {
	auto& ei = bsdata<spelli>::elements[id];
	switch(id) {
	case CureLightWound:
		heal(ei.effect.roll());
		break;
	default:
		if(ei.duration!=Instant && ei.duration!=Permanent)
			enchant(id, getduration(ei.duration, level));
		break;
	}
	return true;
}