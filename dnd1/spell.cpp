#include "bsdata.h"
#include "creature.h"
#include "condition.h"
#include "pushvalue.h"
#include "rand.h"
#include "scenery.h"
#include "spell.h"
#include "ongoing.h"

void choose_target();

spelln last_spell;
int last_level;

static void dispelling(const spelli::spella& source, variant owner) {
	for(auto v : source)
		dispell(owner, v);
}

static bool allow_dispelling(const spelli::spella& source, const creature* player) {
	for(auto v : source) {
		if(player->is(v))
			return true;
	}
	return false;
}

static bool cast_on_target(bool run, int maximum_count, bool random, bool use_hit_dices) {
	targets.match(&creature::isallowspell, true);
	if(!targets)
		return false;
	if(random)
		zshuffle(targets.begin(), targets.count);
	if(maximum_count && !use_hit_dices) {
		if((int)targets.count > maximum_count)
			targets.count = maximum_count;
	}
	if(run) {
		if(use_hit_dices) {
			for(auto p : targets) {
				if(maximum_count <= 0)
					break;
				p->apply(last_spell, last_level, run);
				auto level = p->get(Level);
				if(!level)
					level = 1;
				maximum_count -= level;
			}
		} else {
			for(auto p : targets)
				p->apply(last_spell, last_level, run);
		}
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

static bool allow_creature(variant v) {
	if(v.iskind<feati>())
		return player->is((feat_s)v.value);
	else if(v.iskind<conditioni>())
		return bsdata<conditioni>::elements[v.value].proc();
	else
		return false;
}

static bool allow_item(variant v) {
	if(v.iskind<conditioni>())
		return bsdata<conditioni>::elements[v.value].proc();
	else
		return false;
}

static bool allow_one_of(const variants& source, bool(*proc)(variant)) {
	for(auto v : source) {
		if(proc(v))
			return true;
	}
	return false;
}

static bool allow_all_of(const variants& source, bool(*proc)(variant)) {
	for(auto v : source) {
		if(!proc(v))
			return false;
	}
	return true;
}

static void filter_targets(const variants& source, bool keep = true) {
	auto push_player = player;
	auto ps = targets.begin();
	auto pe = targets.end();
	for(auto pb = ps; pb < pe; pb++) {
		player = *pb;
		if(allow_all_of(source, allow_creature) != keep)
			continue;
		*ps++ = player;
	}
	player = push_player;
}

bool spelli::isevil() const {
	switch(range) {
	case OneEnemy:
	case OneEnemyTouch:
		return true;
	default:
		return false;
	}
}

static int getduration(durationn d, int level) {
	return bsdata<durationi>::elements[d].roll();
}

bool item::isallowspell() const {
	return const_cast<item*>(this)->apply(last_spell, last_level, false);
}

bool creature::apply(spelln id, int level, bool run) {
	auto& ei = bsdata<spelli>::elements[id];
	auto count = run ? ei.random.roll() : ei.random.maximum();
	if(run) {
		if(ei.range == OneEnemyTouch)
			set(EngageMelee);
		if(save(id, count))
			return false;
	}
	//switch(id) {
	//case CureLightWound:
	//	if(is(Unholy))
	//		return false;
	//	if(!iswounded() && !allow_dispelling(ei.dispell, this))
	//		return false;
	//	if(run)
	//		heal(count);
	//	break;
	//case CauseLightWound:
	//	if(run)
	//		damage(count);
	//	break;
	//case MirrorImages:
	//	if(run) {
	//		auto n = basic.abilities[IllusionCopies] + count;
	//		if(abilities[IllusionCopies] < n)
	//			abilities[IllusionCopies] = n;
	//	}
	//	break;
	//case DeathSpell:
	//	if(is(Unholy))
	//		return false;
	//	if(get(Level) > 7)
	//		return false;
	//	if(run)
	//		kill();
	//	break;
	//default:
	//	if(ei.duration != Instant)
	//		enchant(player, this, ei.enchant, getduration(ei.duration, level));
	//	break;
	//}
	//if(run) {
	//	actid(ei.id, "Apply");
	//	dispelling(ei.dispell, this);
	//}
	return true;
}

bool item::apply(spelln id, int level, bool run) {
	auto& ei = bsdata<spelli>::elements[id];
	//switch(id) {
	//case DetectEvil:
	//	if(!iscursed() || isidentified())
	//		return false;
	//	if(run) {
	//		identified = 1;
	//		identified_magic = 1;
	//	}
	//	break;
	//case DetectMagic:
	//	if(isidentified() || (!ismagic() && !iscursed()))
	//		return false;
	//	if(run)
	//		identified_magic = 1;
	//	break;
	//case ItemRepair:
	//	if(!isbroken())
	//		return false;
	//	if(run)
	//		broken = 0;
	//	break;
	//}
	if(run)
		actid(ei.id, "Apply");
	return true;
}

bool scenery::apply(spelln id, int level, bool run) {
	auto& ei = bsdata<spelli>::elements[id];
	//switch(id) {
	//case CureLightWound:
	//	break;
	//default:
	//	if(ei.duration != Instant && ei.duration != PermanentDuration)
	//		enchant(player, this, id, getduration(ei.duration, level));
	//	break;
	//}
	if(run)
		dispelling(ei.dispell, this);
	return true;
}

bool spell_effect(spelln spell, int level, rangen range, const interval& random, const char* suffix, bool run) {
	pushvalue push_spell(last_spell, spell);
	pushvalue push_level(last_level, level);
	if(run) {
		// First fix melee engage
		if(range == OneEnemyTouch)
			player->set(EngageMelee);
		// Second fix spell casting
		if(suffix)
			player->actid(bsdata<spelli>::elements[spell].id, suffix);
	}
	auto count = random;
	switch(range) {
	case OneAlly:
		targets = creatures;
		targets.matchally(true);
		return cast_on_target(run);
	case OneEnemy:
		targets = creatures;
		targets.matchenemy(true);
		return cast_on_target(run);
	case OneEnemyTouch:
		targets = creatures;
		targets.matchenemy(true);
		targets.match(EngageMelee, true);
		return cast_on_target(run);
		//case AllAlly:
		//	targets = creatures;
		//	targets.matchally(true);
		//	return cast_on_target(run, target.roll(), target, false);
		//case AllEnemies:
		//	targets = creatures;
		//	targets.matchenemy(true);
		//	return cast_on_target(run, target.roll(), target, false);
		//case AllEnemiesHD:
		//	targets = creatures;
		//	targets.matchenemy(true);
		//	return cast_on_target(run, target.roll(), target, true);
		//case OneItem:
		//	items.clear();
		//	items.select(player->backpack());
		//	return cast_on_item(run);
	case AllyItem:
		items.clear();
		for(auto p : creatures) {
			if(p->isally())
				items.select(player->backpack());
		}
		return cast_on_item(run);
	case Enviroment:
		if(!scene)
			return false;
		return scene->apply(last_spell, last_level, run);
	case Scenery:
		return false;
	default:
		return false;
	}
}

bool spell_effect(spelln spell, int level, const char* suffix, bool run) {
	auto& ei = bsdata<spelli>::elements[spell];
	return spell_effect(spell, player->get(Level), ei.range, ei.random, suffix, run);
}

static bool effect_on_targets(int count, bool run) {
	auto& ei = bsdata<spelli>::elements[last_spell];
	filter_targets(ei.filter);
	if(!targets)
		return false;
	if(run) {
		if(!count) {
			auto result = targets.choose("Test");
		}
	}
	return true;
}

bool apply_effect(spelln spell, int level, rangen range, int count, const variants& filter, const char* suffix, bool run) {
	pushvalue push_level(last_level, level);
	pushvalue push_spell(last_spell, spell);
	if(run) {
		// First fix melee engage
		if(range == OneEnemyTouch)
			player->set(EngageMelee);
		// Second fix spell casting
		if(suffix)
			player->actid(bsdata<spelli>::elements[spell].id, suffix);
	}
	switch(range) {
	case OneAlly:
		targets = creatures;
		targets.matchally(true);
		filter_targets(filter);
		return effect_on_targets(count, run);
	case OneEnemy:
		targets = creatures;
		targets.matchenemy(true);
		return effect_on_targets(count, run);
	case OneEnemyTouch:
		targets = creatures;
		targets.matchenemy(true);
		targets.match(EngageMelee, true);
		filter_targets(filter);
		return effect_on_targets(count, run);
		//case AllAlly:
		//	targets = creatures;
		//	targets.matchally(true);
		//	return cast_on_target(run, target.roll(), target, false);
		//case AllEnemies:
		//	targets = creatures;
		//	targets.matchenemy(true);
		//	return cast_on_target(run, target.roll(), target, false);
		//case AllEnemiesHD:
		//	targets = creatures;
		//	targets.matchenemy(true);
		//	return cast_on_target(run, target.roll(), target, true);
		//case OneItem:
		//	items.clear();
		//	items.select(player->backpack());
		//	return cast_on_item(run);
	case AllyItem:
		items.clear();
		for(auto p : creatures) {
			if(p->isally())
				items.select(player->backpack());
		}
		return cast_on_item(run);
	case Enviroment:
		if(!scene)
			return false;
		return scene->apply(last_spell, last_level, run);
	case Scenery:
		return false;
	default:
		return false;
	}
}

bool creature::cast(spelln spell, bool run) {
	pushvalue push_caster(caster, this);
	auto& ei = bsdata<spelli>::elements[spell];
	return spell_effect(spell, get(Level), ei.range, ei.random, "Cast", run);
}

void creature::use(spelln spell) {
	if(spells[spell])
		spells[spell]--;
}

void spella::select(const spellf& source) {
	auto ps = begin();
	auto pe = (spelli**)endof();
	for(auto i = (spelln)0; i < (spelln)128; i = (spelln)(i + 1)) {
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
	for(auto i = (spelln)0; i <= (spelln)128; i = (spelln)(i + 1)) {
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
		if(ei.version || ei.reversed)
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