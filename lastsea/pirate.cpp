#include "main.h"

void pirate::clearactions() {
	action = 0xFF;
	action_additional = 0xFF;
}

void pirate::clear() {
	memset(this, 0, sizeof(*this));
	historyable::clear();
	clearactions();
}

const char* pirate::getavatarst(const void* object) {
	auto p = (pirate*)object;
	return p->avatar;
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
	default:
		return 10;
	}
}

void pirate::information(const char* format, ...) {
	logs::sb.addn("[+");
	actv(logs::sb, format, xva_start(format), false);
	logs::sb.add("]");
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
		auto pv = logs::choose(an, temp);
		if(!pv)
			break;
	}
	rollv(last_bonus);
	sb.clear(); fixroll(sb); information(temp);
}

bool pirate::isuse(int v) const {
	return action == v || action_additional == v;
}

void pirate::setaction(int i) {
	action = i;
}