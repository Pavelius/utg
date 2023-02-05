#include "crt.h"
#include "dice.h"
#include "magic.h"
#include "nameable.h"
#include "range.h"
#include "resid.h"
#include "variant.h"

#pragma once

struct animationi {
	const char*		avatar;
	const char*		ground;
	res::token		wear, thrown;
};
struct weaponi {
	char			bonus, ac, magic;
	dice			damage;
	range_s			range;
};
struct itemi : nameable {
	const char*		id_unidentified;
	magic_s			rate;
	unsigned		count, weight, cost;
	variants		dress, use;
	animationi		animation;
	weaponi			weapon;
};
struct item {
	unsigned short	type;
	unsigned char	count;
	union {
		struct {
			unsigned char broken : 2;
			unsigned char identified : 1;
			unsigned char charges : 5;
		};
		unsigned char data;
	};
	constexpr item() : type(0), count(0), data(0) {}
	const itemi&	geti() const { return bsdata<itemi>::elements[type]; }
};
