#include "main.h"

int harmable::getdistinct() const {
	auto result = 0;
	for(auto v : harm) {
		if(v)
			result++;
	}
	return result;
}