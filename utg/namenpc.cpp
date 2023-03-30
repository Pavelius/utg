#include "namenpc.h"
#include "stringact.h"
#include "stringlist.h"

void namenpc::actv(stringbuilder& sb, const char* format, const char* format_param) const {
	if(!format || format[0] == 0)
		return;
	auto push_name = act_name;
	auto push_gender = act_gender;
	sb.addsep(' ');
	sb.addv(format, format_param);
	act_gender = push_gender;
	act_name = push_name;
}

gender_s namenpc::getgender() const {
	if(nameid == 0xFFFF)
		return NoGender;
	return Male;
}

const char* namenpc::getname() const {
	if(nameid == 0xFFFF)
		return "Noname";
	return bsdata<stringlist>::elements[nameid].name;
}