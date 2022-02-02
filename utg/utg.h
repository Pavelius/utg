#include "answers.h"
#include "crt.h"
#include "draw_figure.h"
#include "flagable.h"
#include "front.h"
#include "menu.h"
#include "point.h"
#include "variant.h"

#pragma once

typedef flagable<4> flaga;

namespace draw {
extern const void*	focus_object;
extern array*		heroes;
extern fngetname	heroes_getavatar;
extern const void*	hilite_object;
extern int			title_width;
void				answerbt(int index, const void* id, const char* title);
void				avatar(int index, const void* object, const char* id);
void				avatarch(int index, const void* object, const char* id);
bool				buttonfd(const char* title);
bool				isnext();
void				label(const char* id, const char* value);
void				label(const char* id, const char* value, const void* object);
void				label(const void* object, const variants& elements, fngetinfo pget);
void				noavatar();
void				setcaret(int x, int y);
void				setnext(fnevent v);
int					start(fnevent proc, bool darkmode, fnevent afterread = 0);
void				texth2(const char* title);
void				vertical(fnevent proc);
}
namespace utg {
namespace callback {
extern fngetinfo	getinfo;
extern fnstatus		getstatus;
}
extern stringbuilder sb;
extern const char*	url_avatars;
const char*			chooseavatar(answers& an, const char* title);
void				chooseavatar(stringbuilder& result, const char* title, const char* mask = 0);
int					choose(const array& source, const char* title, const char* cancel);
int					choose(const array& source, const flaga& mask, const char* title, const char* cancel);
const char*			getchoose(const char* id);
void				getstatus(const void* object, stringbuilder& sb);
}