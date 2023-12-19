#include "crt.h"
#include "creature.h"
#include "pushvalue.h"
#include "scenery.h"
#include "spell.h"
#include "ongoing.h"

BSDATA(spelli) = {
	{"CauseLightWound", {1}, Instant, OneEnemy, {2, 7}},
	{"CauseFear", {1}, Hour2, OneEnemy},
	{"CureLightWound", {1}, Instant, CasterOrAlly, {2, 7}},
	{"Darkness", {1, 1}, Hour2, Enviroment},
	{"DetectEvil", {1}, Instant, AllAllyItems},
	{"DetectMagic", {1, 1}, Instant, AllAllyItems},
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
	{"SpeakWithAnimals", {2, 0}, Hour, Enviroment},
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

void choose_target();

spell_s	last_spell;
int		last_level;

static void dispelling(const slice<spell_s>& source, variant owner) {
	for(auto v : source) {
		if(!v)
			break;
		dispell(owner, v);
	}
}

static bool cast_on_target(bool run, unsigned maximum_count, bool random) {
	targets.match(&creature::isallowspell, true);
	if(!targets)
		return false;
	if(random)
		zshuffle(targets.begin(), targets.count);
	if(maximum_count) {
		if(targets.count > maximum_count)
			targets.count = maximum_count;
	}
	if(run) {
		for(auto p : targets)
			p->apply(last_spell, last_level, run);
	}
	return true;
}

static bool cast_on_target(bool run) {
	targets.match(&creature::isallowspell, true);
	if(!targets)
		return false;
	if(run) {
		pushvalue push(opponent);
		choose_target();
		opponent->apply(last_spell, last_level, run);
	}
	return true;
}

static bool cast_on_item(bool run, unsigned maximum_count, bool random) {
	items.match(&item::isallowspell, true);
	if(!items)
		return false;
	if(random)
		zshuffle(items.begin(), items.count);
	if(maximum_count) {
		if(items.count > maximum_count)
			items.count = maximum_count;
	}
	if(run) {
		for(auto p : items)
			p->apply(last_spell, last_level, run);
	}
	return true;
}

static bool cast_on_item(bool run) {
	items.match(&item::isallowspell, true);
	if(!items)
		return false;
	if(run) {
		pushvalue push(last_item);
		last_item = items.choose(getnm(str(bsdata<spelli>::elements[last_spell].id, "Target")));
		last_item->apply(last_spell, last_level, run);
	}
	return true;
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
	return bsdata<durationi>::elements[d].roll();
}

bool item::isallowspell() const {
	return const_cast<item*>(this)->apply(last_spell, last_level, false);
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
			enchant(player, this, id, getduration(ei.duration, level));
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
			enchant(player, this, id, getduration(ei.duration, level));
		break;
	}
	if(run)
		dispelling(ei.dispell, this);
	return true;
}

bool creature::cast(spell_s spell, int level, bool run) {
	pushvalue push_spell(last_spell, spell);
	pushvalue push_level(last_level, level);
	auto& ei = bsdata<spelli>::elements[last_spell];
	switch(ei.range) {
	case Caster:
		return apply(last_spell, last_level, true);
	case CasterOrAlly:
		targets = creatures;
		targets.matchally(true);
		targets.match(&creature::isplayer, false);
		return cast_on_target(run);
	case OneAlly:
		targets = creatures;
		targets.matchally(true);
		return cast_on_target(run);
	case OneEnemy:
		targets = creatures;
		targets.matchenemy(true);
		return cast_on_target(run);
	case SomeEnemies:
		targets = creatures;
		targets.matchenemy(true);
		return cast_on_target(run, xrand(2, 4), true);
	case AllAlly:
		targets = creatures;
		targets.matchally(true);
		return cast_on_target(run, 0, false);
	case AllEnemies:
		targets = creatures;
		targets.matchenemy(true);
		return cast_on_target(run, 0, false);
	case OneItem:
		items.clear();
		items.select(player->backpack());
		return cast_on_item(run);
	case OneAllyItem:
		items.clear();
		for(auto p : creatures) {
			if(p->isally())
				items.select(player->backpack());
		}
		return cast_on_item(run);
	case AllCasterItems:
		items.clear();
		items.select(player->backpack());
		return cast_on_item(run, 0, false);
	case AllAllyItems:
		items.clear();
		for(auto p : creatures) {
			if(p->isally())
				items.select(player->backpack());
		}
		return cast_on_item(run, ei.count.roll(), ei.count);
	case Enviroment:
		if(!scene)
			return false;
		return scene->apply(last_spell, last_level, run);
	case OneObject:
		return false;
	case OneRandomObject:
		return false;
	default:
		return false;
	}
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