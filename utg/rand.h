#pragma once

#define maprnd(t) t[rand()%(sizeof(t)/sizeof(t[0]))]

extern "C" int rand(void);
extern "C" void srand(unsigned seed); // Set random seed

inline int d20() { return 1 + rand() % 20; }
inline int d4() { return 1 + rand() % 4; }
inline int d6() { return 1 + rand() % 6; }
inline int d100() { return rand() % 100; }
inline int xrand(int n1, int n2) { return n1 + rand() % (n2 - n1 + 1); }

template<class T> inline void zshuffle(T* p, int count) {
	for(int i = 0; i < count; i++) {
		auto j = rand() % count;
		auto v = p[i];
		p[i] = p[j];
		p[j] = v;
	}
}
