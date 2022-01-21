#include "charname.h"
#include "namenpc.h"
#include "stringact.h"

void namenpc::actv(stringbuilder& sbs, const char* format, const char* format_param) {
	stringact sb(sbs, getname(), getgender());
	sb.addsep(' ');
	sb.addv(format, format_param);
	sbs = sb;
}

gender_s namenpc::getgender() const {
	if(nameid == 0xFFFF)
		return NoGender;
	return (gender_s)bsdata<charname>::elements[nameid].conditions[0].value;
}

const char* namenpc::getname() const {
	if(nameid == 0xFFFF)
		return "Noname";
	return bsdata<charname>::elements[nameid].name;
}