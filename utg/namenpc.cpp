#include "namenpc.h"
#include "stringact.h"
#include "stringlist.h"

void namenpc::actv(stringbuilder& sbs, const char* format, const char* format_param) const {
	stringact sb(sbs, getname(), getgender());
	sb.addsep(' ');
	sb.addv(format, format_param);
	sbs = sb;
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