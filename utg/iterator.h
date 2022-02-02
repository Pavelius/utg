#include "crt.h"

#pragma once

typedef unsigned short indext;

template<typename T, typename V = indext>
class iterator {
	struct element {
		const V* data;
		constexpr bool operator!=(const element& v) const { return data != v.data; }
		constexpr T* operator*() const { return bsdata<T>::elements + *data; }
		constexpr void operator++() { data++; }
	};
	const V* data;
	size_t count;
public:
	constexpr iterator(const V* v, size_t count) : data(v), count(count) {}
	template<size_t N> constexpr iterator(const adat<V, N>& v) : data(v.data), count(v.count) {}
	template<size_t N> constexpr iterator(char(&v)[N]) : data(v), count(N) {}
	element	begin() const { return {data}; }
	element	end() const { return {data + count}; }
};
