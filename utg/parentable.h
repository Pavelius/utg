#pragma once

template<typename T>
struct bsdata;

template<typename T>
struct parentable {
	short unsigned	parent;
	constexpr explicit operator bool() const { return parent != 0; }
	constexpr T*	getparent() const { return (parent == 0xFFFF) ? 0 : bsdata<T>::elements + parent; }
	void			setparent(const T* pv) { parent = pv ? pv - bsdata<T>::elements : 0xFFFF; }
};
