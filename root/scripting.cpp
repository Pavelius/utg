#include "ability.h"
#include "answers.h"
#include "harmable.h"
#include "modifier.h"
#include "quest.h"
#include "script.h"
#include "vagabond.h"

template<> void fnscript<abilityi>(int index, int bonus) {
	player->harm[index] += bonus;
}

template<> void fnscript<harmi>(int index, int bonus) {
	switch(modifier) {
	case Inflict: inflict.harm[index] += bonus; break;
	case Suffer: suffer.harm[index] += bonus; break;
	default: break;
	}
}

static void add_new(const char* format) {
	draw::pause();
	answers::console->add(format);
}

void quest::run(int page) {
	if(!answers::console)
		return;
	auto p = findprompt(page);
	const variants* additional = 0;
	while(p) {
		player->act(p->text);
		script::run(p->tags);
		if(additional) {
			script::run(additional);
			additional = 0;
		}
		answers an;
		auto index = p->index;
		auto pe = bsdata<quest>::end();
		for(auto pa = p + 1; pa < pe; pa++) {
			if(pa->index != index)
				continue;
			an.add(pa, pa->text);
		}
		p = (quest*)an.choose(0, getnm("Back"), 1);
		answers::console->clear();
		if(!p)
			break;
		additional = &p->tags;
		p = findprompt(p->next);
		if(!p && additional) {
			script::run(additional);
			additional = 0;
		}
	}
}

BSDATA(script) = {
	{"ChooseVagabond"},
};