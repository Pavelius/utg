#include "bsdata.h"
#include "rand.h"
#include "result.h"

BSDATA(resulti) = {
	{"Fail"},
	{"Success"},
	{"StrongSuccess"},
};
assert_enum(resulti, StrongSuccess)

int	last_roll, last_roll_result, last_roll_bonus;
result_s last_result;

result_s pbta_roll(int b) {
	last_roll_bonus = b;
	last_roll_result = 2 + rand() % 6 + rand() % 6;
	last_roll = last_roll_result + last_roll_bonus;
	if(last_roll >= 10)
		last_result = StrongSuccess;
	else if(last_roll >= 7)
		last_result = Success;
	else
		last_result = Fail;
	return last_result;
}