#include "stringbuilder.h"
#include "strprint.h"
#include "ship.h"
#include "planet.h"
#include "variant.h"

static void add_header(stringbuilder& sb, const char* name) {
	sb.addn("###%1", name);
	sb.addn("---");
}

void ship::getinfo(stringbuilder& sb) const {
	auto& ei = geti();
	add_header(sb, ei.getname());
	sb.addn("%Hull: %1i/%2i", hull, modules[Hull]);
	if(shield)
		sb.addn("%Shield: %1i/%2i", shield, modules[Shield]);
	sb.addn("%Speed: %1i", modules[Engine]);
}

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

BSDATA(strprinti) = {
	{"Planet", planet_text},
};
BSDATAF(strprinti)

void stringbuilder_proc(stringbuilder& sb, const char* identifier) {
	if(print_identifier(sb, identifier))
		return;
	stringbuilder::defidentifier(sb, identifier);
}