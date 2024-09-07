#include "gender.h"
#include "stringbuilder.h"

bool apply_list(const char* identifier, stringbuilder& sb);

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
	{"такая", "такой", "такие"},
	{"леди", "лорд", "лорды"},
	{"ась", "ся", "ись"},
	{"нее", "него", "них"},
	{"она", "он", "они"},
	{"шла", "шел", "шли"},
	{"ая", "ый", "ые"},
	{"ее", "его", "их"},
	{"ей", "ему", "им"},
	{"ла", "", "ли"},
	{"а", "", "и"},
};
}

static bool apply_name(const char* identifier, stringbuilder& sb, const char* name) {
	if(!name)
		return false;
	if(equal(identifier, "герой") || equal(identifier, "name"))
		sb.add(name);
	else if(equal(identifier, "героя"))
		sb.addof(name);
	else if(equal(identifier, "герою"))
		sb.addto(name);
	else
		return false;
	return true;
}

static bool apply_gender(const char* identifier, stringbuilder& sb, gender_s gender) {
	for(auto& e : player_gender) {
		if(!equal(e.female, identifier))
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

bool apply_action(const char* identifier, stringbuilder& sb, const char* name, gender_s gender) {
	if(apply_name(identifier, sb, name))
		return true;
	if(apply_gender(identifier, sb, gender))
		return true;
	return false;
}

const char* act_name;
gender_s act_gender;

void act_custom(stringbuilder& sb, const char* identifier) {
	apply_action(identifier, sb, act_name, act_gender);
}

void stract(stringbuilder& sb, gender_s gender, const char* name, const char* format, const char* format_param) {
	auto push_name = act_name;
	auto push_gender = act_gender;
	auto push_proc = stringbuilder::custom;
	act_name = name;
	act_gender = gender;
	stringbuilder::custom = act_custom;
	sb.addv(format, format_param);
	act_name = push_name;
	act_gender = push_gender;
}