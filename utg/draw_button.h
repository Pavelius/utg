#pragma once

typedef void(*fnevent)();

extern bool button_pressed, button_executed, button_hilited, input_disabled;

void button_clear();
void button_check(unsigned key, bool execute_by_press, bool disabled);
void fire(fnevent proc, long param = 0, long param2 = 0, const void* object = 0);
