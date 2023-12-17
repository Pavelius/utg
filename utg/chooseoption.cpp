#include "answers.h"
#include "chooseoption.h"

chooseoption* last_option;

static void apply_option() {
	if(last_option)
		last_option->proc(true);
}

void choosev(const slice<chooseoption>& options, const char* title, const char* format_param, bool random) {
	answers an;
	for(auto& e : options) {
		if(!e.proc(false))
			continue;
		an.addv(&e, getnm(e.id), format_param);
	}
	last_option = (chooseoption*)(random ? an.random() : an.choose(title));
	apply_option();
}

void choose(const slice<chooseoption>& options, const char* title, ...) {
	choosev(options, title, xva_start(title), false);
}

void chooser(const slice<chooseoption>& options, const char* title, ...) {
	choosev(options, title, xva_start(title), true);
}

void choosef(const slice<chooseoption>& options) {
	last_option = 0;
	for(auto& e : options) {
		if(!e.proc(false))
			continue;
		last_option = &e;
		break;
	}
	apply_option();
}