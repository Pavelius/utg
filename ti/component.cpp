#include "main.h"

bool component::isallow() const {
	if(parent.iskind<playeri>())
		return getbsi(game.active) == parent.value;
	return true;
}