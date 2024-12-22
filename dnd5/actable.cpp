#include "actable.h"
#include "answers.h"
#include "stringvar.h"
#include "speech.h"

void actable::act(const char* format, ...) const {
	if(!answers::console)
		return;
}

bool actable::iskind(variant v) const {
	return kind == v;
}

bool actable::ischaracter() const {
	return true;
}

const char* actable::getname() const {
	if(name_id != 0xFFFF)
		return speech_name(name_id);
	return kind.getname();
}