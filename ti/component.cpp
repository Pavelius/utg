#include "actioncard.h"
#include "player.h"

bool actioncard::isallow() const {
	if(parent.iskind<playeri>())
		return getbsi(player) == parent.value;
	return true;
}