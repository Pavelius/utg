#include "animal.h"
#include "hero.h"

static void create_animal(const animali* p) {
	player->clear();
	player->getanimal();
}

void create_animal(const char* id) {
	auto p = bsdata<animali>::find(id);
	if(!p)
		return;
}