#include "main.h"

const treasurei* treasurei::find(const char* id) {
	for(auto& e : bsdata<treasurei>()) {
		if(equal(e.id, id))
			return &e;
	}
	return 0;
}
