#pragma once

template<typename T>
struct bsdata;

template<typename T, typename V = short unsigned>
struct typeable {
	V					type;
	constexpr explicit operator bool() const { return type != 0; }
	constexpr const T&	geti() const { return bsdata<T>::elements[type]; }
	const char*			getname() const { return geti().getname(); }
	static const char*	getname(const void* object) { return ((typeable*)object)->getname(); }
};
