#include "crt.h"
#include "stringact.h"
#include "stringlist.h"

const char	*act_name, *act_enemy_name;
gender_s	act_gender, act_enemy_gender;

bool apply_list(const char* identifier, stringbuilder& sb);

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

bool apply_name(const char* identifier, stringbuilder& sb, const char* name) {
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

bool apply_gender(const char* identifier, stringbuilder& sb, gender_s gender) {
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

//void act_identifier(stringbuilder& sb, const char* identifier) {
//	if(apply_name(identifier, sb, act_name))
//		return;
//	if(apply_gender(identifier, sb, act_gender))
//		return;
//	if(apply_list(identifier, sb))
//		return;
//	stringbuilder::defidentifier(sb, identifier);
//}
//
//void stract(stringbuilder& sb, gender_s gender, const char* name, const char* format, const char* format_param) {
//	auto push_name = act_name;
//	auto push_gender = act_gender;
//	act_name = name;
//	act_gender = gender;
//	sb.addv(format, format_param);
//	act_gender = push_gender;
//	act_name = push_name;
//}