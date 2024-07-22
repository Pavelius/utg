#pragma once

template<typename T> struct funct {
	typedef void (T::*command)(); // typical object command
	typedef bool (T::*visible)() const; // object visibility
};

template<typename T, funct<T>::visible F>
inline bool fntis(const void* p) { return (((T*)p)->*F)(); }

template<typename T, funct<T>::command F>
inline void fntcm() { (((T*)p)->*F)(); }
