#include "archive.h"
#include "bsreq.h"
#include "draw.h"
#include "game.h"
#include "relation.h"

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

static void serial(archive& a, deck& source) {
	variant v;
	a.set(source.count);
	for(auto& e : source) {
		if(a.writemode) {
			v = e;
			a.set(v);
		} else {
			a.set(v);
			e = v;
		}
	}
}

static bool serial_game_file(const char* id, bool write) {
	char temp[260]; stringbuilder sb(temp); sb.add("saves/%1.sav", id);
	io::file file(temp, write ? StreamWrite : StreamRead);
	if(!file)
		return false;
	archive a(file, write);
	if(!a.signature("SAV"))
		return false;
	if(!a.version(0, 10))
		return false;
	if(!a.signature(bsreq_signature()))
		return false;
	a.set(game);
	a.set(draw::camera);
	a.set(current_relation);
	a.set(bonus_relation);
	serial(a, "Player");
	serial(a, "Province");
	serial(a, "PlayerSite", true);
	serial(a, "MoveOrder", true);
	serial(a, neutral_tactics);
	for(auto& e : bsdata<playeri>())
		serial(a, e.tactics);
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