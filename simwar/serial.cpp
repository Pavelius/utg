#include "archive.h"
#include "bsreq.h"
#include "game.h"

void serial_object(archive& f, array& source, const bsreq* type);
void serial_object(archive& f, void* object, const bsreq* type);

static void serial(archive& f, const char* id, bool clean_all = false) {
	auto p = bsdata<varianti>::find(id);
	if(!p || !p->source)
		return;
	if(!f.writemode && clean_all)
		p->source->clear();
	serial_object(f, *p->source, p->metadata);
}

static bool serial_game_file(const char* id, bool write) {
	char temp[260]; stringbuilder sb(temp); sb.add("saves/%1.sav", id);
	io::file file(temp, write ? StreamWrite : StreamRead);
	if(!file)
		return false;
	archive a(file, write);
	if(!a.signature("SAV"))
		return false;
	if(!a.signature(bsreq_signature()))
		return false;
	a.set(game);
	serial(a, "Player");
	serial(a, "Province");
	serial(a, "Hero");
	serial(a, "PlayerBuilding", true);
	serial(a, "PlayerSite", true);
	serial(a, "Troop", true);
	return true;
}

void gamei::write(const char* id) {
	serial_game_file(id, true);
}

bool gamei::read(const char* id) {
	if(!serial_game_file(id, false))
		return false;
	player = bsdata<playeri>::elements;
	return true;
}