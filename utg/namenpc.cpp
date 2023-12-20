#include "namenpc.h"
#include "stringlist.h"

void namenpc::actv(stringbuilder& sb, const char* format, const char* format_param) const {
	if(!format || format[0] == 0)
		return;
	sb.addsep(' ');
	sb.addv(format, format_param);
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