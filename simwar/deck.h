#pragma once

#include "card.h"
#include "cardaf.h"

enum deck_s : unsigned char {
	TacticCards,
};

struct deck : collection<cardi> {
	void		addall(cardi& v);
	void		create(deck_s v);
	void		matchmask(const cardaf& source, bool keep);
};
extern deck neutral_tactics;
struct decki : entityi {
	deck		cards;
};