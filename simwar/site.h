#include "province.h"

#pragma once

struct sitei : nameable {
	costa				effect, upkeep;
};
struct site {
	const sitei*		type;
	provincei*			province;
	explicit operator bool() const { return type != 0; }
	void				clear();
};
extern sitei* lastsite;
