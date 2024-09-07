#include "creature.h"
#include "stringbuilder.h"
#include "stringvar.h"

void act_custom(stringbuilder& sb, const char* identifier);

static void print_hands(stringbuilder& sb) {
	if(!player)
		return;
	if(!player->wears[Hands])
		sb.add(getnm("Hands"));
	else
		sb.add(player->wears[Hands].getname());
}

BSDATA(stringvari) = {
	{"Hands", print_hands}
};
BSDATAF(stringvari)

static void main_custom(stringbuilder& sb, const char* identifier) {
	if(stringvar_identifier(sb, identifier))
		return;
	act_custom(sb, identifier);
}

void initialize_printer() {
	stringbuilder::custom = main_custom;
}