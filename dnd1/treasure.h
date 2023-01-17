#include "item.h"

#pragma once

struct treasure : adat<item> {
	void add(item it);
	void generate(char symbol);
	void take();
};
inline int d100() { return rand() % 100; }