#include "actable.h"
#include "answers.h"
#include "stringact.h"

void actable::actv(const char* format, const char* format_param) const {
	stract(*answers::console, gender, getname(), format, format_param);
}

void actable::act(const char* format, ...) const {
	actv(format, xva_start(format));
}

const char* actable::getname() const {
	return "Pavel";
}