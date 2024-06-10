#pragma once

#include "nameable.h"

struct cardi;
struct playeri;
struct provincei;
struct landscapei;

struct entityi : nameable {
	cardi*				getcard() const;
	provincei*			getprovince() const { return 0; }
	playeri*			getplayer() const;
	const landscapei*	getlandscape() const;
};