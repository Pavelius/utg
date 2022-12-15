#include "main.h"
#include "quest.h"
#include "script.h"

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

static void choose_player(int bonus) {
}

BSDATA(script) = {
	{"ChoosePlayer", choose_player},
};
BSDATAF(script)