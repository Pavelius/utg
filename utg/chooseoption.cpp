#include "answers.h"
#include "chooseoption.h"

chooseoption* last_option;

static void apply_option() {
	if(last_option)
		last_option->proc(true);
}

static chooseoption* find_option(const slice<chooseoption>& options, const char* id) {
	for(auto& e : options) {
		if(equal(e.id, id))
			return &e;
	}
	return 0;
}

static void choose_first_options() {
	last_option = 0;
	for(auto& e : an) {
		last_option = (chooseoption*)e.value;
		break;
	}
}

static void choose_options(const char* title, bool random) {
	last_option = (chooseoption*)(random ? an.random() : an.choose(title));
}

static void add_options(const char* format_param, const slice<chooseoption>& options) {
	an.clear();
	for(auto& e : options) {
		if(!e.proc(false))
			continue;
		an.addv(&e, getnm(e.id), format_param);
	}
}

void useopt(const slice<chooseoption>& options, const char* id) {
	last_option = find_option(options, id);
	apply_option();
}

void choosev(const slice<chooseoption>& options, const char* title, const char* format_param, bool random) {
	add_options(format_param, options);
	choose_options(title, random);
	apply_option();
}

void choose(const slice<chooseoption>& options, const char* title, ...) {
	choosev(options, title, xva_start(title), false);
}

void chooser(const slice<chooseoption>& options, const char* title, ...) {
	choosev(options, title, xva_start(title), true);
}

void choosef(const slice<chooseoption>& options) {
	add_options(0, options);
	choose_first_options();
	apply_option();
}