#include "draw.h"
#include "itemlay.h"
#include "scenery.h"

scenery* scene;
static scenery* next_scene;

static void play_scene() {
	scene = next_scene; next_scene = 0;
}

void scenery::enter() {
	if(next_scene)
		return;
	next_scene = this;
	draw::setnext(play_scene);
}

bool scenery::haveitems() const {
	variant v = this;
	for(auto& e : bsdata<itemlay>()) {
		if(e.parent == v)
			return true;
	}
	return false;
}