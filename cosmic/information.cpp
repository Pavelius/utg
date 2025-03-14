#include "gender.h"
#include "ship.h"
#include "stringvar.h"

bool apply_action(const char* identifier, stringbuilder& sb, const char* name, gendern gender);

static void addcms(stringbuilder& sb, const char* name, int count) {
	if(!count)
		return;
	if(sb)
		sb.add(", ");
	sb.add(name);
	if(count > 1)
		sb.adds("x%1i", count);
}

static void add_line(stringbuilder& sb, modulen f1, modulen f2) {
	for(auto i = f1; i<=f2; i = (modulen)(i+1))
		addcms(sb, bsdata<modulei>::elements[i].getname(), opponent->modules[i]);
}

static void main_custom(stringbuilder& sb, const char* id) {
	if(apply_action(id, sb, player->getname(), Male))
		return;
	if(stringvar_identifier(sb, id))
		return;
	default_string(sb, id);
}

static void opponent_class(stringbuilder& sb) {
	sb.add(opponent->geti().geti().getname());
}

static void opponent_name(stringbuilder& sb) {
	opponent_class(sb);
	sb.add(" ");
	sb.add(opponent->getname());
}

static void add_weapons(stringbuilder& sb) {
	char temp[512]; stringbuilder sbs(temp); temp[0] = 0;
	add_line(sbs, ShardCannons, RocketLaunchersIII);
	if(temp[0])
		sb.addn("[%1]: %2", getnm("Weapons"), temp);
}

static void opponent_scaner_info(stringbuilder& sb) {
	sb.addn("[%1]: %2 %3", getnm("Class"), opponent->geti().geti().getname(), opponent->getname());
	add_weapons(sb);
}

void initilize_script() {
	stringbuilder::custom = main_custom;
}

BSDATA(stringvari) = {
	{"opponent_class", opponent_class},
	{"opponent_name", opponent_name},
	{"opponent_scaner_info", opponent_scaner_info},
};
BSDATAF(stringvari)