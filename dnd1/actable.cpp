#include "actable.h"
#include "answers.h"
#include "stringact.h"

void actable::actv(stringbuilder& sbo, const char* format, const char* format_param, char separator) const {
	if(!answers::interactive)
		return;
	if(separator)
		sbo.addsep(separator);
	stract(sbo, gender, getname(), format, format_param);
}

void actable::act(const char* format, ...) const {
	actv(*answers::console, format, xva_start(format), ' ');
}

void actable::actn(const char* format, ...) const {
	actv(*answers::console, format, xva_start(format), '\n');
}

bool actable::actid(const char* prefix, const char* suffix, char separator) const {
	char temp[260]; stringbuilder sb(temp);
	sb.add(prefix);
	sb.add(suffix);
	auto format = getnme(temp);
	if(!format)
		return false;
	actv(*answers::console, format, 0, separator);
	return true;
}

void initialize_str() {
	stringbuilder::custom = act_identifier;
}