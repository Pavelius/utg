#include "main.h"

using namespace pathfind;

namespace {
struct pointline {
	point			position;
	char			count;
	char			weight;
};
}

static pointline player6[] = {
	{{2, 0}, 2},
	{{1, 1}, 5},
	{{0, 2}, 6},
	{{1, 3}, 2},
	{{4, 3}, 2},
	{{0, 4}, 6},
	{{1, 5}, 5},
	{{2, 6}, 2},
};

void analize::clear() {
	memset(this, 0, sizeof(*this));
}

int analize::getweight(indext i) const {
	if(i == Blocked)
		return 0;
	return data[i];
}

void analize::setweight(indext i, int v) {
	if(i == Blocked)
		return;
	data[i] = v;
}