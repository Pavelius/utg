#pragma once

extern int last_roll_raw, critical_roll;

bool rolld20(int bonus, int dc, bool fix_roll = false);