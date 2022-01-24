#include "main.h"

void pirate::clearactions() {
	action = 0xFF;
	action_additional = 0xFF;
}

void pirate::clear() {
	memset(this, 0, sizeof(*this));
	historyable::clear();
	clearactions();
	for(auto& v : treasures)
		v = 0xFFFF;
}

int pirate::getnextstar(int value) const {
	auto& ei = getclass();
	auto old_value = 0;
	for(auto v : ei.levelup) {
		if(value >= old_value && value < v)
			return v;
	}
	return 0;
}

void pirate::getpropertyst(const void* object, variant v, stringbuilder& sb) {
	auto p = (pirate*)object;
	int value;
	switch(v.type) {
	case Ability:
		switch(v.value) {
		case Experience:
			value = p->get((ability_s)v.value);
			sb.add("%1i %-From %2i", value, p->getmaximum((ability_s)v.value));
			value = p->getnextstar(value);
			if(value)
				sb.adds("(%1 %2i)", getnm("NextStar"), value);
			break;
		case Stars:
			value = p->get((ability_s)v.value);
			if(value)
				sb.add("%1i", value);
			break;
		default:
			value = p->get((ability_s)v.value);
			sb.add("%1i", value);
			break;
		}
		break;
	}
}

const char* pirate::getavatarst(const void* object) {
	auto p = (pirate*)object;
	auto i = bsdata<pirate>::source.indexof(p);
	if(i == -1)
		return 0;
	static char temp[260];
	stringbuilder sb(temp);
	sb.add("pirate%1i", i + 1);
	return temp;
}

void pirate::generate() {
	chooseclass();
}

bool pirate::match(variant v) const {
	switch(v.type) {
	case Class: return classid == v.value;
	case Gender: return getgender() == v.value;
	default: return false;
	}
}

int	pirate::getmaximum(ability_s v) const {
	switch(v) {
	case Exploration:
	case Brawl:
	case Hunting:
	case Aim:
	case Swagger:
	case Navigation:
		return getclass().maximum[v - Exploration];
	case Experience: return 18;
	case Stars: case Level: return 5;
	default: return 10;
	}
}

void pirate::information(const char* format, ...) {
	utg::sb.addn("[+");
	actv(utg::sb, format, xva_start(format), false);
	utg::sb.add("]");
}

const char* classi::getearn(ability_s v) const {
	switch(v) {
	case Exploration: return exploration;
	case Brawl: return brawl;
	case Hunting: return hunting;
	case Aim: return aim;
	case Swagger: return swagger;
	case Navigation: return navigation;
	default: return 0;
	}
}

void pirate::checkstars() {
	auto pa = getclass().levelup;
	auto cv = get(Experience);
	for(unsigned i = 0; i < sizeof(classi::levelup) / sizeof(classi::levelup[0]); i++) {
		if(cv == pa[i]) {
			set(Stars, get(Stars) + 1);
			break;
		}
	}
}

void pirate::checkexperience(ability_s v) {
	auto pa = getclass().getearn(v);
	auto cv = get(v);
	for(unsigned i = 0; i < sizeof(classi::aim) / sizeof(classi::aim[0]); i++) {
		if(cv == pa[i]) {
			set(Experience, get(Experience) + 1);
			break;
		}
	}
}

void pirate::afterchange(ability_s v) {
	switch(v) {
	case Exploration: case Brawl: case Hunting: case Aim: case Swagger: case Navigation:
		checkexperience(v);
		break;
	case Experience:
		checkstars();
		break;
	}
}

void pirate::set(ability_s v, int i) {
	if(i < 0)
		i = 0;
	auto m = getmaximum(v);
	if(i > m)
		i = m;
	if(abilities[v] != i) {
		if(abilities[v] < i)
			information(getnm("AbilityGrow"), getnm(bsdata<abilityi>::elements[v].id), i - abilities[v]);
		abilities[v] = i;
		afterchange(v);
	}
}

static void fixroll(stringbuilder& sb) {
	sb.add(getnm("YouRoll"), last_result, last_roll, last_bonus);
}

void pirate::roll() {
	char temp[260]; stringbuilder sb(temp);
	last_bonus = get(last_ability);
	while(true) {
		sb.clear();
		sb.add(getnm("YouRollAbility"), getnm(bsdata<abilityi>::elements[last_ability].id), last_bonus);
		answers an;
		an.add(0, getnm("MakeRoll"));
		auto pv = utg::choose(an, temp);
		if(!pv)
			break;
	}
	rollv(last_bonus);
	sb.clear(); fixroll(sb); information(temp);
	if(!last_action)
		return;
	auto stage = last_action->getstage(last_result);
	game.apply(last_action->getoutcome(stage));
}

bool pirate::isuse(int v) const {
	return action == v || action_additional == v;
}

void pirate::setaction(int i) {
	action = i;
}

void pirate::gaintreasure(const treasurei* pv) {
	auto index = bsdata<treasurei>::source.indexof(pv);
	if(index == -1)
		return;
	for(auto& e : treasures) {
		if(e != 0xFFFF)
			continue;
		e = index;
		break;
	}
}

void pirate::gaintreasure() {
	auto pv = game.picktreasure();
	gaintreasure(pv);
}