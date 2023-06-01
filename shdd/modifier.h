#pragma once

enum modifier_s : unsigned char {
	NoModifier,
	Permanent, Enviroment,
};
struct modifieri {
	const char* id;
};
extern unsigned apply_modifiers;
extern modifier_s modifier;
