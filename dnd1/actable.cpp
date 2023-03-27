#include "actable.h"
#include "stringact.h"

void actable::actv(stringbuilder& sbo, const char* format, const char* format_param, char separator) const {
	if(!answers::interactive)
		return;
	if(separator)
		sbo.addsep(separator);
	addact(sbo, getname(), gender, format, format_param);
}