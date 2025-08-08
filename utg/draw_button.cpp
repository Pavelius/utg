#include "draw.h"
#include "draw_button.h"

using namespace draw;

bool button_pressed, button_executed, button_hilited, input_disabled;

void button_clear() {
	button_pressed = false;
	button_hilited = false;
	button_executed = false;
}

void button_check(unsigned key, bool execute_by_press) {
	static rect	button_rect;
	if(input_disabled)
		return;
	rect rc = {caret.x, caret.y, caret.x + width, caret.y + height};
	if(ishilite(rc)) {
		button_hilited = true;
		if(hkey == MouseLeft) {
			if(hpressed)
				button_rect = rc;
			if(hpressed == execute_by_press)
				button_executed = true;
		}
	}
	if(hkey == MouseLeft && !hpressed)
		button_rect.clear();
	if(key && hkey == key)
		button_rect = rc;
	else if(hkey == InputKeyUp) {
		if(button_rect == rc) {
			button_executed = true;
			button_rect.clear();
		}
	}
	if(button_rect == rc)
		button_pressed = true;
}

void fire(fnevent proc, long param, long param2, const void* object) {
	if(button_executed && proc)
		execute(proc, param, param2, object);
}