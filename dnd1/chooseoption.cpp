#include "answers.h"
#include "chooseoption.h"

void chooseoption::choose(const slice<chooseoption>& options, const char* title) {
	answers an;
	for(auto& e : options) {
		if(!e.proc(false))
			continue;
		an.add(&e, getnm(e.id));
	}
	auto p = (chooseoption*)an.choose(title);
	if(p)
		p->proc(true);
}