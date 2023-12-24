#include "actable.h"
#include "answers.h"
#include "creature.h"
#include "gender.h"
#include "pushvalue.h"
#include "speech.h"
#include "scenery.h"
#include "stringvar.h"

bool apply_action(const char* identifier, stringbuilder& sb, const char* name, gender_s gender);
bool apply_list(const char* identifier, stringbuilder& sb);
bool apply_speech(const char* id, stringbuilder& sb);

const char* actable::getname() const {
	if(name != -1)
		return speech_name(name);
	return type.getname();
}

void actable::actv(stringbuilder& sbo, const char* format, const char* format_param, char separator) const {
	if(!answers::interactive)
		return;
	if(separator)
		sbo.addsep(separator);
	pushvalue push(player, static_cast<creature*>(const_cast<actable*>(this)));
	sbo.addv(format, format_param);
}

void actable::act(const char* format, ...) const {
	actv(*answers::console, format, xva_start(format), ' ');
}

void actable::actn(const char* format, ...) const {
	actv(*answers::console, format, xva_start(format), '\n');
}

bool actable::actid(const char* id, const char* suffix, char separator) const {
	char temp[260]; stringbuilder sb(temp);
	sb.add(id);
	sb.add(suffix);
	auto format = getnme(temp);
	if(!format) {
		format = speech_get(temp);
		if(!format)
			return false;
	}
	pushvalue push(player, static_cast<creature*>(const_cast<actable*>(this)));
	actv(*answers::console, format, 0, separator);
	return true;
}

static void def_identifier(stringbuilder& sb, const char* id) {
	if(apply_speech(id, sb))
		return;
	if(stringvar_identifier(sb, id))
		return;
	stringbuilder::defidentifier(sb, id);
}

static void item_identifier(stringbuilder& sb, const char* id) {
	if(last_item) {
		if(apply_action(id, sb, last_item->getname(), Male))
			return;
	}
	def_identifier(sb, id);
}

void item::act(char separator, const char* format, ...) const {
	if(!answers::console)
		return;
	if(separator)
		answers::console->addsep(separator);
	pushvalue push(last_item, (item*)this);
	pushvalue push_proc(stringbuilder::custom, item_identifier);
	answers::console->add(format, xva_start(format));
}

bool item::actid(const char* id, const char* suffix, char separator) const {
	char temp[260]; stringbuilder sb(temp);
	sb.add(id);
	sb.add(suffix);
	auto format = getnme(temp);
	if(!format)
		return false;
	act(separator, format);
	return true;
}

static bool apply_items(const char* id, stringbuilder& sb) {
	if(equal(id, "melee")) {
		sb.add(player->wears[MeleeWeapon].getname());
		return true;
	}
	return false;
}

static void main_act_identifier(stringbuilder& sb, const char* id) {
	if(player) {
		if(apply_items(id, sb))
			return;
		if(apply_action(id, sb, player->getname(), player->gender))
			return;
	}
	def_identifier(sb, id);
}

void printv(char separator, const char* format, const char* format_param) {
	if(!answers::interactive)
		return;
	if(!answers::console)
		return;
	if(separator)
		answers::console->addsep(separator);
	answers::console->addv(format, format_param);
}

void print(char separator, const char* format, ...) {
	printv(separator, format, xva_start(format));
}

void printn(const char* format, ...) {
	printv('\n', format, xva_start(format));
}

void prints(const char* format, ...) {
	printv(' ', format, xva_start(format));
}

bool printa(const char* id, const char* suffix, char separator) {
	char temp[260]; stringbuilder sb(temp);
	sb.add(id);
	sb.add(suffix);
	auto format = getnme(temp);
	if(!format) {
		format = speech_get(temp);
		if(!format)
			return false;
	}
	printv(separator, format, 0);
	return true;
}

void initialize_str() {
	stringbuilder::custom = main_act_identifier;
}

static void get_scenename(stringbuilder& sb) {
	sb.add(scene->getname());
}

BSDATA(stringvari) = {
	{"scenename", get_scenename}
};
BSDATAF(stringvari)