#include "tech.h"

bool requirement::match(const requirement& e) const {
	for(auto i = 0; i < 4; i++) {
		if(required[i] > e.required[i])
			return false;
	}
	return true;
}

int requirement::maximize() const {
	int result = 0;
	for(auto i = Red; i <= Yellow; i = (color_s)(i + 1)) {
		if(result < required[i])
			result = required[i];
	}
	return result;
}