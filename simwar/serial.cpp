#include "archive.h"
#include "game.h"

void serial_object(archive& f, array& source, const bsreq* type);

static void serial(archive& f, const char* id, bool clean_all = false) {
	auto p = bsdata<varianti>::find(id);
	if(!p || !p->source)
		return;
	if(!f.writemode)
		p->source->clear();
	serial_object(f, *p->source, p->metadata);
}

static void serial_game_file(const char* id, bool write) {
	char temp[260]; stringbuilder sb(temp); sb.add("saves/%1.sav", id);
	io::file file(temp, write ? StreamWrite : StreamRead);
	if(!file)
		return;
	archive a(file, write);
	if(!a.signature("SAV"))
		return;
	if(!a.version(0, 1))
		return;
	a.set(game);
	serial(a, "Player");
	serial(a, "Province");
	serial(a, "Hero");
	serial(a, "PlayerBuilding", true);
	serial(a, "Troop", true);
}

void gamei::write(const char* id) {
	serial_game_file(id, true);
}

void gamei::read(const char* id) {
	serial_game_file(id, false);
}