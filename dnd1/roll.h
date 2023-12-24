#pragma once

extern int last_roll, last_roll_raw, critical_roll;
extern "C" int rand();

inline int d100() { return rand() % 100; }
inline int d20() { return 1 + rand() % 20; }
inline int d6() { return 1 + rand() % 6; }
inline int d4() { return 1 + rand() % 4; }

bool rolld20(int bonus, int dc, bool fix_roll = false);