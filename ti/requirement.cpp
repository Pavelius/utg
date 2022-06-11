#include "main.h"

bool requirement::match(const requirement& e) const {
	for(auto i = 0; i < 4; i++) {
		if(required[i] > e.required[i])
			return false;
	}
	return true;
}