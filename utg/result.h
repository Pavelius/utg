#pragma once

enum result_s : unsigned char {
	Fail, Success, StrongSuccess
};
struct resulti {
	const char* id;
};
extern result_s last_result;
extern int		last_roll, last_roll_result, last_roll_bonus;
extern result_s	pbta_roll(int b);