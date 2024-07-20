#include "move.h"

#pragma once

struct statable : moveable {
	char	abilities[Charisma + 1];
	void	apply_ability(int v);
	void	update_player();
};
