#include "main.h"

static void addheader(stringbuilder& sb, const char* format) {
	sb.add("##%1", format);
	sb.addn("---");
}

static void addname(stringbuilder& sb, const char* id) {
	addheader(sb, getnm(id));
}