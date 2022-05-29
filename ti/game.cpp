#include "main.h"

gamei	game;
int		gamei::options;
void*	gamei::result;

void gamei::defhandle(trigger_s trigger, void* result) {
}

void gamei::prepare() {
	players.clear();
	for(auto& e : bsdata<playeri>())
		players.add(&e);
	human = players.data[0];
	active = human;
}

void gamei::choose(trigger_s trigger, const char* title, fnanswer panswer, fnapplyanswer papply) {
	char temp[260]; stringbuilder sb(temp);
	answers an;
	draw::pause();
	while(options > 0) {
		sb.clear();
		sb.add("[!%1]", getnm(title));
		sb.adds(getnm("ChooseOptions"), options);
		an.clear(); panswer(an);
		result = an.choose(temp);
		if(!result)
			break;
		if(!papply())
			game.defhandle(trigger, result);
	}
	draw::pause();
}