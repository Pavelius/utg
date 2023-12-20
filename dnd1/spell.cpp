#include "crt.h"
#include "creature.h"
#include "pushvalue.h"
#include "scenery.h"
#include "spell.h"
#include "ongoing.h"

BSDATA(spelli) = {
	{"CauseLightWound"},
	{"CureLightWound"},
	{"DetectEvil"},
	{"DetectMagic"},
	{"Light"}, {"LightBlindness"}, {"Darkness"},
	{"ProtectionFromEvil"},
	{"PurifyFoodAndWater"},
	{"RemoveFear"}, {"CauseFear"},
	{"ResistCold"},	//
	{"CharmPerson"},
	{"FloatingDisc"},
	{"HoldPortal"},
	{"MagicMissile"},
	{"ReadLanguages"},
	{"ReadMagic"},
	{"Shield"},
	{"Sleep"},
	{"Ventriloquism"}, // Second level spells
	{"Blindness"},
	{"ContinualDarkness"},
	{"ContinualLight"},
	{"DetectInvisibility"},
	{"ESP"},
	{"Invisibility"},
	{"Knock"},
	{"Levitate"},
	{"MirrorImages"},
	{"PhantasmalForce"},
	{"Web"},
	{"WizardLock"},
	{"Bless"},
	{"Blight"},
	{"FindTraps"},
	{"HoldPerson"},
	{"KnownAlignment"},
	{"ResistFire"},
	{"Silence15Radius"},
	{"SnakeCharm"},
	{"SpeakWithAnimals"},
	{"BestowCurse"}, // Three level spells
	{"CauseDisease"},
	{"CureDisease"},
	{"GrowthOfAnimals"},
	{"LocateObject"},
	{"RemoveCurse"},
	{"FlameBlade"},
	{"AntiMagicShell"},
	{"DeathSpell"},
	{"ShrinkSize"}, // Special spell effects
	{"GrowthSize"},
	{"GaseousForm"},
	{"DeathPoison"},
};
assert_enum(spelli, DeathPoison)

void choose_target();

spell_s	last_spell;
int		last_level;

void spell_initialize() {
	for(auto& e : bsdata<spelli>()) {
		if(!e.enchant)
			e.enchant = (spell_s)getbse(e);
	}
}

static void dispelling(const spelli::spella& source, variant owner) {
	for(auto v : source)
		dispell(owner, v);
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

ability_s spelli::getsave() const {
	if(save_negates)
		return save_negates;
	return save_halves;
}

bool spelli::isevil() const {
	switch(range) {
	case OneEnemy:
	case OneEnemyTouch:
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
	if(run) {
		if(ei.range == OneEnemyTouch)
			set(EngageMelee);
		if(save(id))
			return false;
	}
	auto count = run ? ei.count.roll() : ei.count.maximum();
	switch(id) {
	case CureLightWound:
		if(is(Unholy))
			return false;
		if(!iswounded())
			return false;
		if(run)
			heal(count);
		break;
	case CauseLightWound:
		if(run)
			damage(count);
		break;
	case MirrorImages:
		if(run) {
			auto n = basic.abilities[IllusionCopies] + count;
			if(abilities[IllusionCopies] < n)
				abilities[IllusionCopies] = n;
		}
		break;
	default:
		if(ei.duration != Instant)
			enchant(player, this, ei.enchant, getduration(ei.duration, level));
		break;
	}
	if(run) {
		actid(ei.id, "Apply");
		dispelling(ei.dispell, this);
	}
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
			identified_magic = 1;
		}
		break;
	case DetectMagic:
		if(isidentified() || (!ismagic() && !iscursed()))
			return false;
		if(run)
			identified_magic = 1;
		break;
	}
	if(run)
		actid(ei.id, "Apply");
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

static void set_caster_melee() {
}

bool creature::cast(spell_s spell, int level, bool run) {
	pushvalue push_spell(last_spell, spell);
	pushvalue push_level(last_level, level);
	auto& ei = bsdata<spelli>::elements[last_spell];
	if(run) {
		// First fix melee engage
		if(ei.range == OneEnemyTouch) {
			if(!player->is(EngageMelee)) {
				player->actid("CastTouchSpell", "Action");
				player->set(EngageMelee);
			}
		}
		// Second fix spell casting
		actid(ei.id, "Cast");
	}
	switch(ei.range) {
	case Caster:
		return apply(last_spell, last_level, true);
	case CasterOrAlly:
		targets = creatures;
		targets.matchally(true);
		return cast_on_target(run);
	case OneAlly:
		targets = creatures;
		targets.matchally(true);
		targets.matchyou(false);
		return cast_on_target(run);
	case OneEnemy:
		targets = creatures;
		targets.matchenemy(true);
		return cast_on_target(run);
	case OneEnemyTouch:
		set_caster_melee();
		targets = creatures;
		targets.matchenemy(true);
		targets.match(EngageMelee, true);
		return cast_on_target(run);
	case AllAlly:
		targets = creatures;
		targets.matchally(true);
		return cast_on_target(run, ei.targets.roll(), ei.targets);
	case AllEnemies:
		targets = creatures;
		targets.matchenemy(true);
		return cast_on_target(run, ei.targets.roll(), ei.targets);
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
		return cast_on_item(run, ei.targets.roll(), ei.targets);
	case AllAllyItems:
		items.clear();
		for(auto p : creatures) {
			if(p->isally())
				items.select(player->backpack());
		}
		return cast_on_item(run, ei.targets.roll(), ei.targets);
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
	for(auto i = (spell_s)0; i <= LastSpell; i = (spell_s)(i+1)) {
		if(source.is(i)) {
			if(ps < pe)
				*ps++ = bsdata<spelli>::elements + i;
		}
	}
	count = ps - begin();
}

void spella::select(const spellable& source) {
	auto ps = begin();
	auto pe = (spelli**)endof();
	for(auto i = (spell_s)0; i <= LastSpell; i = (spell_s)(i + 1)) {
		if(source.spells[i]) {
			if(ps < pe)
				*ps++ = bsdata<spelli>::elements + i;
		}
	}
	count = ps - begin();
}

void spella::select(int magic, int level) {
	auto ps = begin();
	auto pe = (spelli**)endof();
	for(auto& ei : bsdata<spelli>()) {
		if(ei.alternate || ei.reversed)
			continue;
		if(ei.level[magic] != level)
			continue;
		if(ps < pe)
			*ps++ = &ei;
	}
	count = ps - begin();
}

void spella::match(int magic, int level, bool keep) {
	auto ps = begin();
	for(auto p : *this) {
		if((p->level[magic] == level) != keep)
			continue;
		*ps++ = p;
	}
	count = ps - begin();
}

void spellable::clear() {
	memset(this, 0, sizeof(*this));
}