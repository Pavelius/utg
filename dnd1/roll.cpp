#include "actable.h"
#include "formula.h"
#include "rand.h"
#include "roll.h"

int last_roll_raw, last_roll, critical_roll;

bool rolld20(int bonus, int dc, bool fix_roll) {
	last_roll_raw = d20();
	last_number = last_roll_raw + bonus;
	critical_roll = 0;
	if(last_roll_raw == 20)
		critical_roll = 1;
	else if(last_roll_raw == 1)
		critical_roll = -1;
	auto result = (critical_roll == -1) ? false : last_number >= dc || (critical_roll == 1);
	if(fix_roll) {
		if(critical_roll == -1)
			printn("[-{%1i}]", last_number);
		else if(critical_roll == 1)
			printn("[+{%1i}]", last_number);
		else if(result)
			printn("[{%1i}]", last_number);
		else
			printn("[~{%1i}]", last_number);
	}
	return (critical_roll == -1) ? false : last_number >= dc || (critical_roll == 1);
}