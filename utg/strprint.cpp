#include "crt.h"
#include "strprint.h"

bool print_identifier(stringbuilder& sb, const char* identifier) {
	auto pn = bsdata<strprinti>::find(identifier);
	if(pn) {
		pn->proc(sb);
		return true;
	}
	return false;
}