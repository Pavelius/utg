#include "main.h"
#include "script.h"

bool actioncard::isallow() const {
	if(parent.iskind<playeri>())
		return getbsi(player) == parent.value;
	return true;
}