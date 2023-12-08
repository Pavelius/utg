#pragma once

template<typename T>
struct bsdata;

template<typename T>
struct typeable {
	short unsigned	type;
	constexpr const T& geti() const { return bsdata<T>::elements[type]; }
};
