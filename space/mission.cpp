#include "main.h"

const int maximum_danger = 5;

missioni game;

void missioni::clear() {
	memset(this, 0, sizeof(*this));
	for(auto& e : allies)
		e.clear();
	for(auto& e : enemies)
		e.clear();
	enemy = ally = 0xFF;
}

static squad* newsquad(squad* source) {
	auto pm = source + sizeof(missioni::allies) / sizeof(missioni::allies[0]);
	for(auto p = source; p < pm; p++) {
		if(*p)
			continue;
		return p;
	}
	return 0;
}

void missioni::add(const char* id, bool enemy) {
	auto i = bsdata<varianti>::elements[Squad].found(id, zlen(id));
	if(i == -1)
		return;
	auto p = newsquad(enemy ? enemies : allies);
	if(!p)
		return;
	p->clear();
}

bool missioni::dangerous() {
	if(danger < maximum_danger) {
		danger++;
		return false;
	} else {
		danger = 0;
		return true;
	}
}

bool missioni::lucky() {
	if(luck < maximum_danger) {
		luck++;
		return false;
	} else {
		luck = 0;
		return true;
	}
}

void missioni::beforemove() {
}

void missioni::apply(variantsl source) {
	for(auto v : source)
		apply(v);
}

void missioni::apply(variant v) {
	switch(v.type) {
	case Prefix:
		switch(v.value) {
		case Plus: multiplier = 1; break;
		case Minus: multiplier = -1; break;
		default: break;
		}
		break;
	default:
		break;
	}
}

void actv(stringbuilder& sb, const char* format, const char* format_param, char add_sep = '\n') {
	sb.addv(format, format_param);
}