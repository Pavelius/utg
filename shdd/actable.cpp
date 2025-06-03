#include "actable.h"
#include "answers.h"
#include "bsdata.h"
#include "speech.h"

void actable::actv(const char* format, const char* format_param) const {
	if(!answers::console)
		return;
	if(answers::console->begin()[0])
		answers::console->addsep('\n');
	answers::console->addv(format, format_param);
}

void actable::act(const char* format, ...) const {
	actv(format, xva_start(format));
}

const char* actable::getname() const {
	return bsdata<speech::element>::elements[name].name;
}