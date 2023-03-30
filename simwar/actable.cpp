#include "actable.h"
#include "stringact.h"

void actable::actv(stringbuilder& sb, const char* format, const char* format_param, const char* name, gender_s gender) {
	auto push_name = act_name;
	auto push_gender = act_gender;
	act_name = name;
	act_gender = gender;
	sb.addv(format, format_param);
	act_name = push_name;
	act_gender = push_gender;
}

void actable::act(stringbuilder& sb, const char* format, ...) const {
	sb.addsep(' ');
	actv(sb, format, xva_start(format), getname(), gender);
}