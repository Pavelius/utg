#include "crt.h"
#include "stringact.h"
#include "stringlist.h"

namespace {
struct gender_change_string {
	const char*	female;
	const char*	male;
	const char*	multiply;
};
static gender_change_string player_gender[] = {
	{"госпожа", "господин", "господа"},
	{"женщина", "мужчина", "господа"},
	{"стерва", "ублюдок", "ублюдки"},
	{"миледи", "милорд", "милорды"},
	{"леди", "лорд", "лорды"},
	{"такая", "такой", "такие"},
	{"она", "он", "они"},
	{"шла", "шел", "шли"},
	{"нее", "него", "них"},
	{"ась", "ся", "ись"},
	{"ая", "ый", "ые"},
	{"ей", "ему", "им"},
	{"ла", "", "ли"},
	{"ее", "его", "их"},
	{"а", "", "и"},
};
struct stringact : stringbuilder {
	const char*	name;
	gender_s	gender;
	stringact(const stringbuilder& v, const char* name, gender_s gender) : stringbuilder(v), name(name), gender(gender) {}
	void		addidentifier(const char* identifier) override;
};
}

static unsigned get_count(stringlist* pb) {
	auto pe = bsdata<stringlist>::end();
	auto id = pb->id;
	auto result = 0;
	while(pb < pe && pb->id == id)
		result++;
	return result;
}

static bool apply_list(const char* identifier, stringbuilder& sb) {
	for(auto& e : bsdata<stringlist>()) {
		if(equal(e.id, identifier)) {
			auto count = get_count(&e);
			sb.add((&e)[rand() % count].name);
			return true;
		}
	}
	return false;
}

static bool apply_gender(const char* identifier, stringbuilder& sb, gender_s gender) {
	for(auto& e : player_gender) {
		if(strcmp(e.female, identifier) != 0)
			continue;
		if(gender == NoGender)
			sb.add(e.multiply);
		else if(gender == Female)
			sb.add(e.female);
		else
			sb.add(e.male);
		return true;
	}
	return false;
}

void stringact::addidentifier(const char* identifier) {
	if(name && (equal(identifier, "герой") || equal(identifier, "name")))
		add(name);
	else if(name && strcmp(identifier, "героя") == 0)
		addof(name);
	else if(name && strcmp(identifier, "герою") == 0)
		addto(name);
	else {
		if(apply_gender(identifier, *this, gender))
			return;
		if(apply_list(identifier, *this))
			return;
		stringbuilder::addidentifier(identifier);
	}
}

void addact(stringbuilder& sbs, const char* name, gender_s gender, const char* format, const char* format_param) {
	stringact sb(sbs, name, gender);
	sb.addv(format, format_param);
	sbs = sb;
}