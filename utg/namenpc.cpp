#include "bsdata.h"
#include "namenpc.h"
#include "groupname.h"

void namenpc::actv(stringbuilder& sb, const char* format, const char* format_param) {
	if(!format || format[0] == 0)
		return;
	sb.addsep(' ');
	sb.addv(format, format_param);
}

gendern namenpc::getgender() const {
	if(nameid == 0xFFFF)
		return NoGender;
	return Male;
}

const char* namenpc::getname() const {
	if(nameid == 0xFFFF)
		return "Noname";
	return bsdata<groupname>::elements[nameid].name;
}