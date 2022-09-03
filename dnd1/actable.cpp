#include "stringact.h"
#include "main.h"

void actable::actv(stringbuilder& sbo, const char* format, const char* format_param, char separator) const {
	if(!answers::interactive)
		return;
	stringact sb(sbo, name, gender);
	if(separator)
		sb.addsep(separator);
	sb.addv(format, format_param);
	sbo = sb;
}