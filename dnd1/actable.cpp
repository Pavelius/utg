#include "stringact.h"
#include "main.h"

void actable::actv(stringbuilder& sbo, const char* format, const char* format_param) const {
	if(!answers::interactive)
		return;
	stringact sb(sbo, getnm(id), gender);
	sb.addv(format, format_param);
	sbo = sb;
}