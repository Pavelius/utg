#include "crt.h"
#include "game.h"

gamei game;

void gamei::clear() {
	memset(this, 0, sizeof(*this));
}