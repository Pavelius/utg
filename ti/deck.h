#include "entitya.h"

#pragma once

enum deck_s : unsigned char {
	ActionCards, AgendaCards, ObjectivesCards, SecretObjectivesCards
};
struct decki : entity {
	entitya		cards;
	void		create();
};
extern decki* last_deck;