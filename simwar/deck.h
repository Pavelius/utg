#pragma once

#include "card.h"

enum deck_s : unsigned char {
	TacticCards,
};

struct deck : collection<cardi> {
};
struct decki : entityi {
	deck		cards;
};