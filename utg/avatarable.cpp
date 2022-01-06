#include "avatarable.h"
#include "stringbuilder.h"

void avatarable::setavatar(const char* v) {
	stringbuilder sb(avatar); sb.add(v);
}