#pragma once

template<typename T>
struct bsdata;

template<typename T>
struct ownerable {
	short unsigned		owner;
	constexpr explicit operator bool() const { return owner != 0; }
	constexpr T*		getowner() const { return (owner == 0xFFFF) ? 0 : bsdata<T>::elements + owner; }
	void				setowner(const T* pv) { owner = pv ? pv - bsdata<T>::elements : 0xFFFF; }
};