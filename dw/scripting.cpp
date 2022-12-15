#include "main.h"
#include "quest.h"
#include "script.h"

static void fix_value(const char* id, int value) {
}

template<> void fnscript<abilityi>(int index, int bonus) {
}

static void choose_player(int bonus) {
}

void quest::run(int index) {
	if(!answers::console)
		return;
	auto p = findprompt(index);
	while(p) {
		player->act(p->text);
		script::run(p->tags);
		answers an;
		auto index = p->index;
		auto pe = bsdata<quest>::end();
		for(auto pa = p + 1; pa < pe; pa++) {
			if(pa->index != index)
				continue;
			if(!pa->isanswer())
				continue;
			an.add(pa, pa->text);
		}
		p = (quest*)an.choose(0, 0, 1);
		if(!p)
			break;
		if(p->tags)
			script::run(p->tags);
		p = findprompt(p->next);
		answers::console->clear();
	}
	draw::pause();
}

BSDATA(script) = {
	{"ChoosePlayer", choose_player},
};
BSDATAF(script)