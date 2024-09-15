#include "calendar.h"
#include "player.h"
#include "province.h"
#include "stringbuilder.h"

#pragma once

struct gamei : calendari {
	int			maximum_fame;
	static bool read(const char* id);
	static void write(const char* id);
};
extern gamei game;

void initialize_game();
int script_count(int counter);

