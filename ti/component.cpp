#include "main.h"
#include "player.h"
#include "script.h"

bool actioncard::isallow() const {
	if(parent.iskind<playeri>())
		return getbsi(player) == parent.value;
	return true;
}