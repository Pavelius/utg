#pragma once

extern int last_roll, last_roll_raw, critical_roll;
extern "C" int rand();

bool rolld20(int bonus, int dc, bool fix_roll = false);