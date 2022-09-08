#include "answers.h"
#include "chooseoption.h"

void chooseoption::choosev(const slice<chooseoption>& options, const char* title, const char* format_param, bool random) {
	answers an;
	for(auto& e : options) {
		if(!e.proc(false))
			continue;
		an.addv(&e, getnm(e.id), format_param);
	}
	auto p = (chooseoption*)(random ? an.random() : an.choose(title));
	if(p)
		p->proc(true);
}