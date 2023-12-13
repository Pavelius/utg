#include "creature.h"
#include "crt.h"
#include "stringact.h"
#include "stringbuilder.h"
#include "strprint.h"

static void print_hands(stringbuilder& sb) {
	if(!player)
		return;
	if(!player->wears[Hands])
		sb.add(getnm("Hands"));
	else
		sb.add(player->wears[Hands].getname());
}

BSDATA(strprinti) = {
	{"hands", print_hands}
};

static void main_custom(stringbuilder& sb, const char* identifier) {
	if(print_identifier(sb, identifier))
		return;
	act_identifier(sb, identifier);
}