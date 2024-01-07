#include "crt.h"
#include "roll.h"

typedef int (*fncompare)(const void* p1, const void* p2);
char roll_result[20];

static void roll_dices(size_t c, int d) {
	for(size_t i = 0; i < sizeof(roll_result) / sizeof(roll_result[0]); i++) {
		if(i < c)
			roll_result[i] = 1 + rand() % d;
		else
			roll_result[i] = 0;
	}
}

static int compare_best(const void* p1, const void* p2) {
	return *((char*)p1) - *((char*)p2);
}

static int compare_worst(const void* p1, const void* p2) {
	return *((char*)p1) - *((char*)p2);
}

static int calculate_result(int k) {
	auto r = 0;
	for(auto i = 0; i < k; i++)
		r += roll_result[i];
	return r;
}

static int roll_dices(int c, int d, int k, fncompare proc) {
	roll_dices(c, d);
	qsort(roll_result, c, sizeof(roll_result[0]), proc);
	return calculate_result(k);
}

static int roll_dices(int c, int d, int p, int b, int w) {
	if(b)
		return roll_dices(c, d, b, compare_best) + p;
	else if(w)
		return roll_dices(c, d, w, compare_worst) + p;
	else {
		roll_dices(c, d);
		return calculate_result(c) + p;
	}
}

int rolli::roll() const {
	if(m)
		return roll_dices(c, d, p, b, w) * m;
	return roll_dices(c, d, p, b, w);
}