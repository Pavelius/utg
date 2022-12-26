#include "crt.h"

#pragma once

template<typename T> struct bsenum {
	static constexpr int begin() { return 0; }
	static constexpr int end() { return bsdata<T>::source.getcount(); }
};