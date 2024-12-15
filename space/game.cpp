#include "bsdata.h"
#include "game.h"
#include "ship.h"
#include "slice.h"

gamei game;

void gamei::clear() {
	memset(this, 0, sizeof(*this));
}

ship* gamei::getship() const {
	return bsdata<ship>::elements + game.player_id;
}

void gamei::setship(const ship* v) {
	if(!v)
		player_id = 0;
	else
		player_id = v - bsdata<ship>::elements;
}