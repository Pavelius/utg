#include "bsdata.h"
#include "namenpc.h"
#include "speech.h"

//void namenpc::actv(stringbuilder& sb, const char* format, const char* format_param) {
//	if(!format || format[0] == 0)
//		return;
//	sb.addsep(' ');
//	sb.addv(format, format_param);
//}

const char* namenpc::getname() const {
	if(name_id == 0xFFFF)
		return "Noname";
	return bsdata<speech::element>::elements[name_id].name;
}