#include "actable.h"
#include "answers.h"
#include "creature.h"
#include "io_stream.h"
#include "gender.h"
#include "pushvalue.h"
#include "speech.h"
#include "scenery.h"
#include "stringvar.h"

bool apply_action(const char* identifier, stringbuilder& sb, const char* name, gendern gender);
bool apply_list(const char* identifier, stringbuilder& sb);
bool apply_speech(const char* id, stringbuilder& sb);

void printv(char separator, const char* format, const char* format_param, bool uppercase) {
	if(!answers::interactive)
		return;
	if(!answers::console)
		return;
	if(separator)
		answers::console->addsep(separator);
	auto p = (const char*)answers::console->get();
	answers::console->addv(format, format_param);
	if(uppercase) {
		auto ps = (char*)p;
		ps[0] = upper_symbol(p[0]);
	}
}

void print(char separator, const char* format, ...) {
	printv(separator, format, xva_start(format), true);
}

void printn(const char* format, ...) {
	printv('\n', format, xva_start(format), true);
}

void prints(const char* format, ...) {
	printv(' ', format, xva_start(format), true);
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
	printv(separator, format, 0, true);
	return true;
}

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
	auto name = getnm(id);
	if(name) {
		sb.add(name);
		return;
	}
	default_string(sb, id);
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

void initialize_str() {
	stringbuilder::custom = main_act_identifier;
}

static int group_size(int count) {
	if(count <= 1)
		return 0;
	else if(count <= 3)
		return 1;
	else if(count <= 7)
		return 2;
	else
		return 3;
}

static const char* phrase(const char* id, int param) {
	auto p = speech_find(id);
	if(!p)
		return 0;
	return speech_get(p, param);
}

static void get_scenename(stringbuilder& sb) {
	sb.add(scene->getname());
}

static void get_monster(stringbuilder& sb) {
	if(encountered_monster)
		sb.add(encountered_monster->getlook()->getname());
}

static void get_monsters(stringbuilder& sb) {
	if(encountered_monster) {
		if(encountered_count > 1) {
			auto gs = phrase("GroupSize", group_size(encountered_count));
			if(gs) {
				sb.add(gs);
				//if(encountered_monster->is(Large))
				//	sb.adds("%-LargePl");
				sb.add(" ");
				auto p = sb.get();
				sb.addof(encountered_monster->getlook()->getname());
				*p = lower_symbol(*p);
			}
		} else {
			auto p = sb.get();
			sb.addof(encountered_monster->getlook()->getname());
			*p = lower_symbol(*p);
		}
	}
}

const classi& actable::geti() const {
	if(type.iskind<classi>())
		return bsdata<classi>::elements[type.value];
	return bsdata<classi>::elements[0];
}

monsteri* actable::getmonster() const {
	if(type.iskind<monsteri>())
		return bsdata<monsteri>::elements + type.value;
	return 0;
}

BSDATA(stringvari) = {
	{"scenename", get_scenename},
	{"monster", get_monster},
	{"monsters", get_monsters},
};
BSDATAF(stringvari)