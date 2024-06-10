#pragma once

#include "nameable.h"

struct playeri;

struct entityi : nameable {
	playeri*		player;
	playeri*		getplayer() const { return player; }
};