#include "ability.h"
#include "nameable.h"

#pragma once

struct playeri;
struct provincei;
struct uniti;

struct entity : nameable {
	playeri*		player;
	entity*			location;
	playeri*		getplayer() const { return player; }
	provincei*		getprovince() const;
	uniti*			getunit() const;
};