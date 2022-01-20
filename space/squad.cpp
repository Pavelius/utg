#include "stringact.h"
#include "main.h"

BSDATAC(squadi, 128)

void squad::actv(stringbuilder& sbs, const char* format, const char* format_param) {
	stringact sb(sbs, getname(), getgender());
	sb.addsep(' ');
	sb.addv(format, format_param);
	sbs = sb;
}

void squad::clear() {
	memset(this, 0, sizeof(*this));
	type = 0xFF;
}

void squad::create(int kind) {
	type = kind;
	tags = geti().tags;
	injury = getmaximum(Injury);
	gear = getmaximum(Gear);
	morale = getmaximum(Morale);
	exhause = getmaximum(Exhause);
}

void squad::getinfo(stringbuilder& sb) const {
}

bool squad::isallow(move_s v) const {
	switch(v) {
	case EngageMelee:
		return distance == Close && is(Close);
	case TargetSomeone:
		return distance == Far && is(Far);
	default:
		return true;
	}
}