#include "answers.h"
#include "avatarable.h"
#include "crt.h"
#include "draw_figure.h"
#include "flagable.h"
#include "menu.h"
#include "point.h"
#include "widget.h"
#include "variant.h"

#pragma once

typedef flagable<4> flaga;

namespace draw {
extern const void* focus_object;
extern array* heroes;
extern fngetname heroes_getavatar;
extern const void* hilite_object;
extern int title_width;
void avatar(int index, const void* object, const char* id);
void avatarch(int index, const void* object, const char* id, fnevent press_event);
bool buttonfd(const char* title);
bool isnext();
void information(const char* format, ...);
void label(const char* id, const char* value);
void label(const char* id, const char* value, const void* object);
void label(const char* id, const char* value, const void* object, fnevent proc);
void label(const void* object, const variants& elements, fngetinfo pget);
void noavatar();
void setnext(fnevent v);
int	start(fnevent proc, fnevent afterread = 0);
void texth2(const char* title);
void vertical(fnevent proc);
void warning(const char* format, ...);
}
namespace utg {
namespace callback {
extern fngetinfo getinfo;
extern fnstatus	getstatus;
}
extern stringbuilder sb;
extern const char* url_avatars;
const char* getchoose(const char* id);
void		getstatus(const void* object, stringbuilder& sb);
}