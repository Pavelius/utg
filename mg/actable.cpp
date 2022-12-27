#include "actable.h"
#include "answers.h"
#include "groupname.h"
#include "stringact.h"

static char console[1024];
stringbuilder sb(console);
answers an;
adat<const void*, 32> marked;

const char* actable::getname() const {
	return (name == 0xFFFF) ? 0 : bsdata<groupname>::elements[name].name;
}

void actable::actv(stringbuilder& sa, const char* format, const char* format_param, char separator, const char* name, gender_s gender) {
	stringact sb(sa, name, gender);
	sb.addsep(separator);
	sb.addv(format, format_param);
	sa = sb;
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