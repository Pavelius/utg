#include "main.h"

BSDATA(actioni) = {
	{"Shield"},
	{"Retaliate"},
	{"Move"},
	{"Attack"},
	{"Push"},
	{"Pull"},
	{"Heal"},
	{"DisarmTrap"},
	{"Loot"},
	{"Kill"},
	{"Bless"},
	{"Curse"},
	{"RecoverDiscard"},
	{"RecoverLost"},
	{"Discard"},	
};
assert_enum(actioni, Discard)

static modifiera	modifiers;
static special_s	special;
static creaturea	targets;

static void clear_modifiers() {
	memset(modifiers, 0, sizeof(modifiers));
	modifiers[Target] = 1;
}

static variant* skip_condition(variant* p, variant* pe) {
	while(p < pe) {
		if(p->iskind<conditioni>())
			break;
		p++;
	}
	return p;
}

variant* creaturei::getmodifiers(variant* p, variant* pe, char* modifiers) {
	while(p < pe) {
		if(p->iskind<conditioni>()) {
			auto pc = bsdata<conditioni>::elements + p->value;
			if(pc->proc(p->counter, pc->param))
				p = getmodifiers(p, pe, modifiers);
			else
				p = skip_condition(p, pe);
		} else if(p->iskind<targeti>()) {
			if(active)
				active->apply((target_s)p->value);
			p++;
		} else if(p->iskind<modifieri>()) {
			if(modifiers)
				modifiers[p->value] += p->counter;
			p++;
		} else
			break;
	}
	return p;
}

void creaturei::getmodifiers(stringbuilder& sb) {
	auto i = 0;
	auto pb = sb.get();
	for(auto& e : bsdata<modifieri>()) {
		if(!modifiers[i])
			continue;
		if(pb[0])
			sb.add(", ");
		sb.add("%1 %2i", getnm(e.id), modifiers[i]);
	}
}

int creaturei::get(modifier_s i) {
	return modifiers[i];
}

void creaturei::apply(target_s type) {
	switch(type) {
	case TargetAlliesAround:
		targets.select();
		targets.match(Hostile, !is(Hostile));
		pathfind::clearpath();
		pathfind::blockwalls();
		pathfind::makewave(getindex());
		targets.range(1);
		break;
	case TargetAlly:
		targets.select();
		targets.match(Hostile, is(Hostile));
		break;
	case TargetEnemiesAround:
		targets.select();
		targets.match(Hostile, !is(Hostile));
		pathfind::clearpath();
		pathfind::blockwalls();
		pathfind::makewave(getindex());
		targets.range(1);
		break;
	case TargetSelf:
		targets.clear();
		targets.add(this);
		break;
	default:
		break;
	}
}

void creaturei::apply(action_s type) {
	switch(type) {
	case Attack:
		targets.clear();
		for(auto i = 0; i < modifiers[Target]; i++) {
			auto enemy = chooseenemy();
			if(!enemy)
				break;
			targets.add(enemy);
			attack(*enemy, modifiers[Bonus], modifiers[Pierce]);
		}
		break;
	case Move:
		move(modifiers[Bonus]);
		break;
	case Pull:
		for(auto p : targets)
			p->pull(getindex(), modifiers[Bonus]);
		break;
	case Push:
		for(auto p : targets)
			p->push(getindex(), modifiers[Bonus]);
		break;
	case Heal:
		for(auto p : targets)
			p->heal(modifiers[Bonus]);
		break;
	case DisarmTrap:
		break;
	case Loot:
		loot(modifiers[Bonus]);
		break;
	case Kill:
		break;
	case Bless:
		break;
	case Curse:
		break;
	case RecoverDiscard:
		break;
	case RecoverLost:
		break;
	case Discard:
		break;
	}
}

int creaturei::getactive(action_s id, variant* p, variant* pe) const {
	int result = 0;
	while(p < pe) {
		if(p->iskind<durationi>()) {
			break;
		} else if(p->iskind<actioni>()) {
			auto pid = (action_s)p->value;
			if(id == pid)
				result += p->counter;
			modifiera modifiers = {};
			p = getmodifiers(p + 1, pe, modifiers);
			if(id == pid)
				result += modifiers[Bonus];
		} else if(p->iskind<elementi>()) {
			p = getmodifiers(p + 1, pe, modifiers);
		} else if(p->iskind<targeti>())
			p++;
		else if(p->iskind<statei>()) {
			p++;
		} else
			break;
	}
	return result;
}

int creaturei::getactive(action_s id) const {
	if(bsdata<monsteri>::have(parent)) {
		auto p = (monsteri*)parent;
		auto pc = monsterdeck::get(p->getdeck()).current();
		if(!pc)
			return 0;
		return getactive(id, pc->abilities.begin(), pc->abilities.end());
	}
	return 0;
}

void creaturei::apply(variants source) {
	auto p = source.begin();
	auto pe = source.end();
	while(p < pe) {
		if(p->iskind<durationi>()) {
			activecardi::add(this, playercardi::last, (duration_s)p->value, p->counter, {p + 1, pe});
			break;
		} else if(p->iskind<actioni>()) {
			clear_modifiers();
			auto type = (action_s)p->value;
			auto pm = getmonster();
			if(pm) {
				if(type == Attack || type == Move)
					modifiers[Bonus] += pm->get(type);
				modifiers[Range] += pm->get(Range);
			}
			modifiers[Bonus] += p->counter;
			p = getmodifiers(p + 1, pe, modifiers);
			apply(type);
			addexperience(modifiers[Experience]);
			wait_all();
		} else if(p->iskind<elementi>()) {
			game.set((element_s)p->value);
			p = getmodifiers(p + 1, pe, modifiers);
		} else if(p->iskind<targeti>()) {
			apply((target_s)p->value);
			p++;
		} else if(p->iskind<statei>()) {
			auto i = (state_s)p->value;
			p = getmodifiers(p + 1, pe, modifiers);
			for(auto p : targets)
				set(i);
		} else
			break;
	}
}