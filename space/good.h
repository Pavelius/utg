#include "item.h"

#pragma once

enum good_s : unsigned char {
	Foods, Medicaments, Technics, Luxury, Minerals, Alchogols, Weapons, Drugs,
};
struct goodi : itemi {
	unsigned	flags;
};