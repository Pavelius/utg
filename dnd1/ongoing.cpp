#include "main.h"

ongoing* ongoing::find(spell_s id, const creature* p) {
	for(auto& e : bsdata<ongoing>()) {
		if(e.id == id && e.owner == p)
			return &e;
	}
	return 0;
}