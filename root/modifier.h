#pragma once

enum modifier_s : unsigned char {
	NoModifier, Inflict, Suffer,
};
struct modifieri {
	const char* id;
};
extern modifier_s modifier;
