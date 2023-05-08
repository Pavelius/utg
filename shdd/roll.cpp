#include "crt.h"

int roll_result;

int roll(int value, int advantages) {
	roll_result = xrand(1, value);
	if(advantages > 0) {
		auto r1 = xrand(1, value);
		if(r1 > roll_result)
			roll_result = r1;
	} else if(advantages < 0) {
		auto r1 = xrand(1, value);
		if(r1 < roll_result)
			roll_result = r1;
	}
	return roll_result;
}

int roll20(int advantages) {
	return roll(20, advantages);
}