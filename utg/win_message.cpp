#include "win.h"

void dlgmsg(const char* title, const char* format) {
	MessageBoxA(GetActiveWindow(), format, title, MB_OK);
}