#include "actable.h"
#include "stringact.h"

void actable::actv(stringbuilder& sbo, const char* format, const char* format_param, char separator) const {
	if(!answers::interactive)
		return;
	if(separator)
		sbo.addsep(separator);
	stract(sbo, gender, getname(), format, format_param);
}

void initialize_str() {
	stringbuilder::custom = act_identifier;
}