#pragma once

template<typename T>
struct scopei {
	scopei<T>*			previous;
	const T*			value;
	scopei(const T& object) : value(&object), previous(active) { last = &object; }
	~scopei() { last = value; active = previous; }
	static scopei<T>*	active;
	static const T*		last;
};
