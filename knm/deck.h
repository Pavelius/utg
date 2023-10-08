#include "entitya.h"

#pragma once

enum deck_s : unsigned char {
	TacticsDeck, LawsDeck, ObjectivesDeck, SecretObjectivesDeck, UpgradesDeck
};
struct decki : entity {
	entitya		cards;
	void		create();
};
extern decki* last_deck;