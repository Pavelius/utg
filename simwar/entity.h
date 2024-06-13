#pragma once

#include "nameable.h"
#include "collection.h"

struct cardi;
struct playeri;
struct provincei;
struct landscapei;

struct entityi : nameable {
	const cardi*		getcard() const;
	const short*		geteffect() const;
	const landscapei*	getlandscape() const;
	playeri*			getplayer() const;
	provincei*			getprovince() const { return 0; }
	const short*		getupkeep() const;
};
typedef collection<entityi> entitya;