#include "crt.h"
#include "ship.h"

ship* ship::add(const char* id, point position) {
	auto pi = bsdata<shipi>::find(id);
	if(!pi)
		return 0;
	auto p = bsdata<ship>::addz();
	p->type = (unsigned char)getbsi(pi);
	p->position = position;
	p->priority = 2;
	p->alpha = 255;
	return p;
}