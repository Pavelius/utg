#pragma once

#include "nameable.h"

struct cardi;
struct playeri;
struct provincei;
struct landscapei;

struct entityi : nameable {
	const cardi*		getcard() const { return 0; }
	const landscapei*	getlandscape() const;
	playeri*			getplayer() const;
	provincei*			getprovince() const { return 0; }
};