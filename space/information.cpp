#include "stringbuilder.h"
#include "ship.h"
#include "planet.h"
#include "variant.h"

struct stringproc {
	const char*	id;
	fnprint		proc;
};

static void planet_text(stringbuilder& sb) {
	if(!last_planet)
		sb.add(getnm("NoPlanet"));
	else
		sb.add(last_planet->getname());
}

static void ship_text(stringbuilder& sb) {
	if(!last_ship)
		sb.add(getnm("NoShip"));
}

BSDATA(stringproc) = {
	{"Planet", planet_text},
};
BSDATAF(stringproc)

void stringbuilder_proc(stringbuilder& sb, const char* id) {
	auto pf = bsdata<stringproc>::find(id);
	if(pf) {
		pf->proc(sb);
		return;
	}
	sb.defidentifier(sb, id);
}