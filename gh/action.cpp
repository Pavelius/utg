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
static target_s		target;
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
	case TargetAllyAround:
	case TargetAlly:
		targets.select();
		targets.match(Hostile, is(Hostile));
		break;
	case TargetEnemyAround:
		targets.select();
		targets.match(Hostile, !is(Hostile));
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
		target = TargetEnemy;
		break;
	case Move:
		move(modifiers[Bonus]);
		break;
	case Pull:
		break;
	case Push:
		break;
	case Heal:
		heal(modifiers[Bonus]);
		break;
	case DisarmTrap:
		break;
	case Loot:
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
			modifiers[Bonus] += p->counter;
			p = getmodifiers(p + 1, pe, modifiers);
			apply(type);
			addexperience(modifiers[Experience]);
			draw::waitall();
		} else if(p->iskind<elementi>()) {
			game.set((element_s)p->value);
			p = getmodifiers(p + 1, pe, modifiers);
		} else if(p->iskind<statei>()) {
			auto i = (state_s)p->value;
			p = getmodifiers(p + 1, pe, modifiers);
			for(auto p : targets)
				set(i);
		} else
			break;
	}
}