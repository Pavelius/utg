#include "gender.h"
#include "stringbuilder.h"

namespace {
struct gender_change_string {
	const char*	female;
	const char*	male;
	const char*	multiply;
};
static gender_change_string player_gender[] = {
	{"�������", "��������", "�������"},
	{"�������", "�������", "�������"},
	{"������", "�������", "�������"},
	{"������", "������", "�������"},
	{"�����", "�����", "�����"},
	{"����", "����", "�����"},
	{"���", "��", "���"},
	{"���", "����", "���"},
	{"���", "��", "���"},
	{"���", "���", "���"},
	{"��", "��", "��"},
	{"��", "���", "��"},
	{"��", "���", "��"},
	{"��", "", "��"},
	{"�", "", "�"},
};
}

static bool apply_name(const char* identifier, stringbuilder& sb, const char* name) {
	if(!name)
		return false;
	if(equal(identifier, "�����") || equal(identifier, "name"))
		sb.add(name);
	else if(equal(identifier, "�����"))
		sb.addof(name);
	else if(equal(identifier, "�����"))
		sb.addto(name);
	else
		return false;
	return true;
}

static bool apply_gender(const char* identifier, stringbuilder& sb, gendern gender) {
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

bool apply_action(const char* identifier, stringbuilder& sb, const char* name, gendern gender) {
	if(apply_name(identifier, sb, name))
		return true;
	if(apply_gender(identifier, sb, gender))
		return true;
	return false;
}