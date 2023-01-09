#include "calendar.h"
#include "crt.h"

static const char* names[] = {
	"January", "February",
	"March", "April", "May",
	"June", "July", "August",
	"September", "October", "November",
	"December"
};

void calendari::getname(stringbuilder& sb) const {
	sb.add("%1 %2i", getnm(names[turn % 12]), getyear());
}