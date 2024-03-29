#include "actable.h"
#include "answers.h"

static char console[1024];
stringbuilder sb(console);
adat<const void*, 32> marked;

void stract(stringbuilder& sb, gender_s gender, const char* name, const char* format, const char* format_param);

void actable::actv(stringbuilder& sa, const char* format, const char* format_param, char separator, const char* name, gender_s gender) {
	if(!format || !format[0])
		return;
	sa.addsep(' ');
	stract(sa, gender, name, format, format_param);
}

bool ismarked(const void* p) {
	for(auto m : marked) {
		if(m == p)
			return true;
	}
	return false;
}

void markvalue(const void* p) {
	if(ismarked(p))
		return;
	marked.add(p);
}