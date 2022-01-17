#include "bsreq.h"
#include "result.h"

BSDATA(resulti) = {
	{"Fail"},
	{"PartialSuccess"},
	{"Success"},
	{"CriticalSuccess"},
};
assert_enum(resulti, CriticalSuccess)

int last_roll;
result_s last_result;

result_s pbaroll(int b) {
	last_roll = 2 + rand() % 6 + rand() % 6 + b;
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