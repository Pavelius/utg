#include "crt.h"
#include "creature.h"
#include "scenery.h"
#include "spell.h"
#include "ongoing.h"

BSDATA(spelli) = {
	{"CauseLightWound", {1}, Instant, OneEnemy, {2, 7}},
	{"CauseFear", {1}, Hour2, OneEnemy},
	{"CureLightWound", {1}, Instant, CasterOrAlly, {2, 7}},
	{"Darkness", {1, 1}, Hour2, Enviroment},
	{"DetectEvil", {1}, Instant, OneRandomItem},
	{"DetectMagic", {1, 1}, Instant, OneRandomItem},
	{"Light", {1, 1}, Hour2, Enviroment},
	{"ProtectionFromEvil", {1, 1}, Hour2, Caster},
	{"PurifyFoodAndWater", {1}, PermanentDuration, OneItem},
	{"RemoveFear", {1}, Instant, CasterOrAlly, {}, {CauseFear}},
	{"ResistCold", {1}, Hour, CasterOrAlly},
	//
	{"CharmPerson", {0, 1}, Instant, OneEnemy},
	{"FloatingDisc", {0, 1}, Hour, Caster},
	{"HoldPortal", {0, 1}, Turn2d6, OneObject},
	{"MagicMissile", {0, 1}, Instant, OneEnemy, {2, 7}},
	{"ReadLanguages", {0, 1}, Turn2, Caster},
	{"ReadMagic", {0, 1}, Turn, Caster},
	{"Shield", {0, 1}, Turn2, Caster},
	{"Sleep", {0, 1}, Turn4d4, SomeEnemies},
	{"Ventriloquism", {0, 1}, Turn2, Enviroment},
	{"Blindness", {3, 2}, PermanentDuration, OneEnemy},
	{"ContinualDarkness", {3, 2}, PermanentDuration, Enviroment, {}, {ContinualLight, Light}, Darkness},
	{"ContinualLight", {3, 2}, PermanentDuration, Enviroment, {}, {ContinualDarkness, Darkness}, Light},
	{"DetectInvisibility", {0, 2}, Instant, AllEnemies, {}, {Invisibility}},
	{"ESP", {0, 2}, Hour2, Caster},
	{"Invisibility", {0, 2}, PermanentDuration, CasterOrAlly},
	{"Knock", {0, 2}, Instant, Caster},
	{"Levitate", {0, 2}, Hour, Caster},
	{"MirrorImages", {0, 2}, Hour, Caster},
	{"PhantasmalForce", {0, 2}, Hour, Enviroment},
	{"Web", {0, 2}, Hour8, Caster},
	{"WizardLock", {0, 2}, PermanentDuration, Caster},
	{"Bless", {2, 0}, Hour, AllAlly},
	{"Blight", {2, 0}, Hour, AllEnemies},
	{"FindTraps", {2, 0}, Instant, OneRandomObject},
	{"HoldPerson", {2, 3}, Instant, OneEnemy},
	{"KnownAlignment", {2, 0}, Instant, Enviroment},
	{"ResistFire", {2, 0}, Hour, CasterOrAlly},
	{"Silence15Radius", {2, 0}, Hour2, Enviroment},
	{"SnakeCharm", {2, 0}, Turn1d4p1, SomeEnemies},
	{"SpeakWithAnimals", {2, 0}, Hour, EncounterReaction},
	{"BestowCurse", {3, 4}, PermanentDuration, OneEnemy},
	{"CauseDisease", {3, 0}, PermanentDuration, OneEnemy},
	{"CureDisease", {3, 0}, Instant, CasterOrAlly, {}, {CauseDisease, Blindness}},
	{"GrowthOfAnimals", {3, 0}, Hour2, CasterOrAlly},
	{"LocateObject", {3, 0}, Hour, Caster},
	{"RemoveCurse", {3, 4}, Instant, CasterOrAlly, {}, {BestowCurse}},
	{"FlameBlade", {3, 0}, Turn, Caster},
	{"AntiMagicShell", {6, 0}, Hour2, Caster},
	{"DeathSpell", {6, 0}, Instant, Caster},
	{"ShrinkSize", {}, Turn, Caster},
	{"GrowthSize", {}, Turn, Caster},
	{"GaseousForm", {}, Turn, Caster},
	{"DeathPoison", {}, Turn, OneEnemy},
};
assert_enum(spelli, DeathPoison)

spell_s spell;

static void dispelling(const slice<spell_s>& source, variant owner) {
	for(auto v : source) {
		if(!v)
			break;
		dispell(owner, v);
	}
}

bool spelli::isevil() const {
	switch(range) {
	case OneEnemy:
	case SomeEnemies:
	case AllEnemies:
		return true;
	default:
		return false;
	}
}

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
		if(run)
			heal(ei.effect.roll());
		break;
	case MirrorImages:
		if(run) {
			auto n = basic.abilities[IllusionCopies] + ei.effect.roll();
			if(abilities[IllusionCopies] < n)
				abilities[IllusionCopies] = n;
		}
		break;
	default:
		if(ei.duration != Instant && ei.duration != PermanentDuration)
			enchant(this, id, getduration(ei.duration, level));
		break;
	}
	if(run)
		dispelling(ei.dispell, this);
	return true;
}

bool item::apply(spell_s id, int level, bool run) {
	auto& ei = bsdata<spelli>::elements[id];
	switch(id) {
	case DetectEvil:
		if(!iscursed() || isidentified())
			return false;
		if(run) {
			identified = 1;
			player->actid(ei.id, "Success");
		}
		break;
	}
	return true;
}

bool scenery::apply(spell_s id, int level, bool run) {
	auto& ei = bsdata<spelli>::elements[id];
	switch(id) {
	case CureLightWound:
		break;
	default:
		if(ei.duration != Instant && ei.duration != PermanentDuration)
			enchant(this, id, getduration(ei.duration, level));
		break;
	}
	if(run)
		dispelling(ei.dispell, this);
	return true;
}

void spella::select(const spellf& source) {
	auto ps = begin();
	auto pe = (spelli**)endof();
	for(auto i = 0; i <= 127; i++) {
		if(source.is(i)) {
			if(ps < pe)
				*ps = bsdata<spelli>::elements + i;
		}
	}
	count = ps - begin();
}

void spella::select(const spellable& source) {
	auto ps = begin();
	auto pe = (spelli**)endof();
	for(auto i = 0; i <= 127; i++) {
		if(source.spells[i]) {
			if(ps < pe)
				*ps = bsdata<spelli>::elements + i;
		}
	}
	count = ps - begin();
}