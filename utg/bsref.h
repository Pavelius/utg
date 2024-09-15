#pragma once

#include "bsdata.h"

template<typename T>
class bsref {
	short unsigned data;
public:
	constexpr bsref() : data(0) {}
	constexpr bsref(const T* p) : data(bsdata<T>::source.indexof(p)) {}
	constexpr operator T*() const { return (data == 0xFFFF) ? 0 : bsdata<T>::elements + data; }
	constexpr bool operator==(const bsref& v) const { return data == v.data; }
	constexpr bool operator!=(const bsref& v) const { return data != v.data; }
	constexpr void clear() { data = 0xFFFF; }
	constexpr T* get() const { return (data == 0xFFFF) ? 0 : bsdata<T>::elements + data; }
};