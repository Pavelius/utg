#include "draw_object.h"
#include "main.h"

void scenarioi::prepare(int stage) const {
	for(auto& e : tiles) {
		if(!e)
			continue;
		e.type->creating(e.position, e.inverse);
		e.updateui();
	}
}