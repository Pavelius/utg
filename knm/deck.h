#include "entitya.h"

#pragma once

enum deck_s : unsigned char {
	TacticsDeck, LawsDeck, ObjectivesDeck, SecretObjectivesDeck
};
struct decki : entity {
	entitya		cards;
	void		create();
};