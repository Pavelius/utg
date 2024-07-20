#pragma once

enum ability_s : unsigned char;

struct classi {
	const char*		id;
	ability_s		abilities[6];
	int				damage;
};
