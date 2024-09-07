#include "avatarable.h"
#include "stringbuilder.h"
#include "slice.h"

void avatarable::clear() {
	memset(this, 0, sizeof(*this));
}

void avatarable::setavatar(const char* v) {
	stringbuilder sb(avatar); sb.add(v);
}