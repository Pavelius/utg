#include "building.h"
#include "calendar.h"
#include "player.h"
#include "province.h"
#include "stringbuilder.h"

#pragma once

struct gamei : calendari {
	void initialize();
	static void read(const char* id);
	static void write(const char* id);
};
extern gamei game;

