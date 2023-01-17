#include "actable.h"
#include "stringact.h"

void actable::actv(stringbuilder& sbo, const char* format, const char* format_param, char separator) const {
	if(!answers::interactive)
		return;
	stringact sb(sbo, getname(), gender);
	if(separator)
		sb.addsep(separator);
	sb.addv(format, format_param);
	sbo = sb;
}