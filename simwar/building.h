#include "bsref.h"
#include "province.h"
#include "nameable.h"

#pragma once

struct buildingi : nameable {
	costa effect, upkeep, cost;
};
struct building {
	bsref<buildingi> type;
	bsref<provincei> parent;
	static building* add(bsref<buildingi> type, bsref<provincei> parent);
	const buildingi* get() const { return type; }
	const provincei* getprovince() const { return parent; }
};