#pragma once

struct tilei {
	short unsigned	frame;
	int				param;
	short unsigned	index;
	static const tilei*	find(int param);
};
