#include "main.h"
#include "stringact.h"

void nameable::actv(stringbuilder& sbs, const char* format, const char* param) const {
	auto name = getnm(id);
	stringact sb(sbs, getnm(id), gender);
	sb.addv(format, param);
}