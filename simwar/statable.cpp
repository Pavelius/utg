#include "main.h"

statable::operator bool() const {
	for(auto v : data) {
		if(v)
			return true;
	}
	return false;
}

void statable::operator+=(const statable& v) {
	for(size_t i = 0; i < sizeof(data) / sizeof(data[0]); i++)
		data[i] += v.data[i];
}

void statable::operator-=(const statable& v) {
	for(size_t i = 0; i < sizeof(data) / sizeof(data[0]); i++)
		data[i] -= v.data[i];
}

bool statable::operator>=(const statable& v) const {
	for(size_t i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
		if(data[i] < v.data[i])
			return false;
	}
	return true;
}

bool statable::operator<=(const statable& v) const {
	for(size_t i = 0; i < sizeof(data) / sizeof(data[0]); i++) {
		if(data[i] > v.data[i])
			return false;
	}
	return true;
}