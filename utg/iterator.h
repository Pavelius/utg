#include "crt.h"

#pragma once

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
//void aindex_remove(indext* data, size_t count, int i);
//bool aindex_is(indext* data, size_t count, int i);
//template<typename T, int N>
//class aindex : adat<indext, N> {
//	int getindex(const void* p) { return bsdata<T>::source.indexof(p); }
//public:
//	void add(const T* p) { auto i = getindex(p); if(i != -1) adat<indext, N>::add(i); }
//	bool is(const T* p) const { return aindex_is(p); }
//	void remove(const T* p) { aindex_remove(adat<indext, N>::data, adat<indext, N>::count, getindex(p)); }
//	constexpr iterable<T, indext> begin() const { return adat<indext, N>::data; }
//	constexpr iterable<T, indext> end() const { return adat<indext, N>::data + adat<indext, N>::count; }
//};
