#include "game.h"
#include "slice.h"

gamei game;

void gamei::clear() {
	memset(this, 0, sizeof(*this));
}