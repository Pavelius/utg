#pragma once

#include "adat.h"
#include "bsdata.h"

typedef unsigned short indext;

template<typename T, typename V = indext>
class iterable {
	const V* data;
public:
	constexpr iterable(const V* data) : data(data) {}
	constexpr bool operator!=(const iterable& v) const { return data != v.data; }
	constexpr T* operator*() const { return bsdata<T>::elements + *data; }
	constexpr void operator++() { data++; }
};
template<typename T, typename V = indext>
class iterator {
	const V* data;
	size_t count;
public:
	constexpr iterator(const V* v, size_t count) : data(v), count(count) {}
	template<size_t N> constexpr iterator(const adat<V, N>& v) : data(v.data), count(v.count) {}
	template<size_t N> constexpr iterator(char(&v)[N]) : data(v), count(N) {}
	iterable<T, V> begin() const { return data; }
	iterable<T, V> end() const { return data + count; }
};