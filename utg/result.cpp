#include "bsreq.h"
#include "result.h"

BSDATA(resulti) = {
	{"Fail"},
	{"PartialSuccess"},
	{"Success"},
	{"CriticalSuccess"},
};
assert_enum(resulti, CriticalSuccess)

int last_roll, last_roll_result, last_roll_bonus;
result_s last_result;

result_s pbaroll(int b) {
	last_roll_bonus = b;
	last_roll_result = 2 + rand() % 6 + rand() % 6;
	last_roll = last_roll_result + last_roll_bonus;
	if(last_roll <= 6)
		last_result = Fail;
	else if(last_roll >= 12)
		last_result = CriticalSuccess;
	else if(last_roll >= 10)
		last_result = Success;
	else
		last_result = PartialSuccess;
	return last_result;
}