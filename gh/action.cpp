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

static char			modifiers[Target + 1];
static special_s	special;
static target_s		target;
static creaturea	targets;

static void clear_modifiers() {
	memset(modifiers, 0, sizeof(modifiers));
	modifiers[Target] = 1;
}

static variant* skip_modifiers(variant* p, variant* pe) {
	while(p < pe) {
		if(p->iskind<modifieri>()) {
			p++;
			continue;
		} else
			break;
	}
	return p;
}

static variant* add_modifier(variant* p, variant* pe, bool add_states) {
	while(p < pe) {
		if(p->iskind<conditioni>()) {
			auto pc = bsdata<conditioni>::elements + p->value;
			if(pc->proc(p->counter, pc->param))
				p = add_modifier(p, pe, true);
			else
				p = skip_modifiers(p, pe);
		} else if(p->iskind<modifieri>()) {
			modifiers[p->value] += p->counter;
			p++;
		} else
			break;
	}
	return p;
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
			p = add_modifier(p + 1, pe, false);
			apply(type);
			addexperience(modifiers[Experience]);
		} else if(p->iskind<elementi>())
			game.set((element_s)p->value);
		else if(p->iskind<statei>()) {
			auto i = (state_s)p->value;
			for(auto p : targets)
				set(i);
		} else
			break;
	}
}