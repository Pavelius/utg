#include "actable.h"
#include "answers.h"
#include "creature.h"
#include "stringact.h"

void actable::actv(stringbuilder& sbo, const char* format, const char* format_param, char separator) const {
	if(!answers::interactive)
		return;
	if(separator)
		sbo.addsep(separator);
	stract(sbo, gender, getname(), format, format_param);
}

void actable::act(const char* format, ...) const {
	actv(*answers::console, format, xva_start(format), ' ');
}

void actable::actn(const char* format, ...) const {
	actv(*answers::console, format, xva_start(format), '\n');
}

bool actable::actid(const char* prefix, const char* suffix, char separator) const {
	char temp[260]; stringbuilder sb(temp);
	sb.add(prefix);
	sb.add(suffix);
	auto format = getnme(temp);
	if(!format)
		return false;
	actv(*answers::console, format, 0, separator);
	return true;
}

static bool act_weapon(stringbuilder& sb, const char* id) {
	if(equal(id, "melee")) {
		sb.add(player->wears[MeleeWeapon].getname());
		return true;
	}
	return false;
}

static void main_act_identifier(stringbuilder& sb, const char* id) {
	if(act_weapon(sb, id))
		return;
	act_identifier(sb, id);
}

void initialize_str() {
	stringbuilder::custom = main_act_identifier;
}