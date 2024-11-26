#include "bsdata.h"
#include "creature.h"
#include "condition.h"
#include "pushvalue.h"
#include "math.h"
#include "rand.h"
#include "party.h"
#include "scenery.h"
#include "script.h"
#include "spell.h"
#include "ongoing.h"

void choose_target();
const char* get_header();

typedef void(*fnrunable)(const variants&);

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

static bool allow_creature(variant v) {
	if(v.iskind<feati>())
		return player->is((feat_s)v.value);
	else if(v.iskind<conditioni>())
		return bsdata<conditioni>::elements[v.value].proc();
	else if(v.iskind<monsteri>())
		return player->getmonster() == (bsdata<monsteri>::elements + v.value);
	else
		return false;
}

static bool allow_item(variant v) {
	if(v.iskind<conditioni>())
		return bsdata<conditioni>::elements[v.value].proc();
	else
		return false;
}

static void filter_targets(const variants& source, bool keep = true) {
	pushvalue push_player(player);
	auto ps = targets.begin();
	auto pe = targets.end();
	for(auto pb = ps; pb < pe; pb++) {
		player = *pb;
		if(allow_all_of(source, allow_creature) != keep)
			continue;
		*ps++ = player;
	}
	targets.count = ps - targets.begin();
}

static void filter_items(const variants& source, bool keep = true) {
	pushvalue push(last_item);
	auto ps = items.begin();
	auto pe = items.end();
	for(auto pb = ps; pb < pe; pb++) {
		last_item = *pb;
		if(allow_all_of(source, allow_item) != keep)
			continue;
		*ps++ = last_item;
	}
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

static void apply_use(const spelli& ei) {
	script_run(ei.use);
}

static void apply_duration(variant caster, creaturea targets, spelln spell, durationn duration) {
	if(duration == Instant || duration==PermanentDuration)
		return;
	auto rounds = bsdata<durationi>::elements[duration].roll();
	for(auto p : targets)
		enchant(caster, p, spell, rounds);
}

static void apply_targets(const variants& source) {
	pushvalue push(player);
	for(auto p : targets) {
		player = p;
		script_run(source);
	}
}

static void apply_items(const variants& source) {
	pushvalue push(last_item);
	for(auto p : items) {
		last_item = p;
		script_run(source);
	}
}

static bool effect_on_targets(int count, bool run) {
	auto& ei = bsdata<spelli>::elements[last_spell];
	filter_targets(ei.filter);
	if(!targets)
		return false;
	if(run) {
		if(!count) {
			choose_target();
			auto index = targets.find(opponent);
			if(index != -1)
				iswap(targets.data[index], targets.data[0]);
			count = 1;
		} else
			zshuffle(targets.data, targets.count);
	}
	if(targets.count > (size_t)count)
		targets.count = count;
	apply_targets(ei.instant);
	apply_duration(player, targets, last_spell, ei.duration);
	for(auto p : targets)
		dispelling(ei.dispell, p);
	apply_use(ei);
	return true;
}

static bool effect_on_item(int count, bool run) {
	auto& ei = bsdata<spelli>::elements[last_spell];
	filter_items(ei.filter);
	if(!items)
		return false;
	if(run) {
		if(!count) {
			auto result = items.choose(getnm(str(bsdata<spelli>::elements[last_spell].id, "Target")));
			auto index = items.find(result);
			if(index != -1)
				iswap(items.data[index], items.data[0]);
			count = 1;
		} else
			zshuffle(items.data, items.count);
	}
	if(items.count > (size_t)count)
		items.count = count;
	apply_items(ei.instant);
	apply_use(ei);
	return true;
}

static bool effect_on_scene(bool run) {
	auto& ei = bsdata<spelli>::elements[last_spell];
	apply_use(ei);
	return true;
}

static bool effect_on_object(int count, bool run) {
	auto& ei = bsdata<spelli>::elements[last_spell];
	apply_use(ei);
	return true;
}

bool apply_effect(spelln spell, int level, rangen range, int count, const char* suffix, bool run) {
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
	party.abilities[EffectCount] = 0;
	switch(range) {
	case OneAlly:
		targets = creatures;
		targets.matchally(true);
		return effect_on_targets(count, run);
	case OneEnemy:
		targets = creatures;
		targets.matchenemy(true);
		return effect_on_targets(count, run);
	case OneEnemyTouch:
		targets = creatures;
		targets.matchenemy(true);
		targets.match(EngageMelee, true);
		return effect_on_targets(count, run);
	case AllyItem:
		items.clear();
		for(auto p : creatures) {
			if(p->isally())
				items.select(player->backpack());
		}
		return effect_on_item(count, run);
	case Enviroment:
		if(!scene)
			return false;
		return effect_on_scene(run);
	case Scenery:
		return effect_on_object(count, run);
	default:
		return false;
	}
}

bool apply_effect(spelln spell, int level, const char* suffix, bool run) {
	auto& ei = bsdata<spelli>::elements[spell];
	return apply_effect(spell, level, ei.range, ei.random.roll(), suffix, run);
}

bool creature::cast(spelln spell, bool run) {
	pushvalue push_caster(caster, this);
	return apply_effect(spell, get(Level), "Cast", run);
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