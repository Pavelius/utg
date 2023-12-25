#pragma once

enum reaction_s : unsigned char {
	Hostile, Unfriendly, NeutralReaction, Indifferent, Friendly
};
extern reaction_s reaction;