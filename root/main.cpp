#include "io_stream.h"
#include "main.h"

static void right_panel() {
	draw::propertybar();
}

static void choose_avatars() {
	answers an; char temp[260];
	for(io::file::find fn(logs::url_avatars); fn; fn.next()) {
		auto p = fn.name();
		if(p[0] == '.')
			continue;
		if(szpmatch(p, "NOPORT*"))
			continue;
		szfnamewe(temp, p);
		an.add(0, temp);
	}
	for(auto& e : an)
		e.value = (void*)e.text;
	auto result = logs::chooseavatar(an, "Как вы выглядите?");
}

static void generate_character() {
	char temp[16]; stringbuilder sb(temp);
	logs::interactive = true;
	logs::url = "northmen";
	game.clear();
	game.suffer.add(Injury, 2);
	game.inflict.add(Injury, 1);
	creature player = {};
	player.id = "Umberto";
	player.abilities[Might] += 2;
	player.abilities[Cunning] -= 1;
	player.move(EngageMelee);
	player.apply(game);
	logs::pause();
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	answers::beforepaint = right_panel;
	quest::read("rules/Moves.txt");
	messagei::read("rules/MoveMessages.txt");
	return draw::utg::run(generate_character, true);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}