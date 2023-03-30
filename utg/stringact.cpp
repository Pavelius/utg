#include "crt.h"
#include "stringact.h"
#include "stringlist.h"

const char*	act_name;
gender_s	act_gender;

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

static bool apply_name(const char* identifier, stringbuilder& sb) {
	if(!act_name)
		return false;
	if(equal(identifier, "герой") || equal(identifier, "name"))
		sb.add(act_name);
	else if(equal(identifier, "героя"))
		sb.addof(act_name);
	else if(equal(identifier, "герою"))
		sb.addto(act_name);
	else
		return false;
	return true;
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

void act_identifier(stringbuilder& sb, const char* identifier) {
	if(apply_name(identifier, sb))
		return;
	if(apply_gender(identifier, sb, act_gender))
		return;
	if(apply_list(identifier, sb))
		return;
	stringbuilder::defidentifier(sb, identifier);
}