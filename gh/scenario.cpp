#include "draw_object.h"
#include "main.h"

void scenarioi::prepare(int stage) const {
	for(auto& e : tiles) {
		if(!e)
			continue;
		e.updateui();
	}
}