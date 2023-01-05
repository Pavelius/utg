#include "building.h"

building* building::add(bsref<buildingi> type, bsref<provincei> parent) {
	auto p = bsdata<building>::add();
	p->type = type;
	p->parent = parent;
	return p;
}