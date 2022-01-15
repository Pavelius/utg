#include "answers.h"
#include "crt.h"
#include "draw_figure.h"
#include "flagable.h"
#include "front.h"
#include "menu.h"
#include "point.h"
#include "variant.h"

#pragma once

struct widget {
	const char*		id;
	fnevent			proc;
};
namespace draw {
typedef void (*fnstatus)(const void* object, stringbuilder& sb);
typedef void (*fngetinfo)(const void* object, variant v, stringbuilder& sb);
extern const void* focus_object;
extern const void* hilite_object;
extern figure hilite_type;
extern fnstatus pstatus;
extern int title_width;
void answerbt(int index, const void* id, const char* title);
void avatar(const char* id, const void* object);
bool buttonfd(const char* title);
void label(const char* id, const char* value);
void label(const char* id, const char* value, const void* object);
void label(const void* object, const variants& elements, fngetinfo pget);
void noavatar();
void propertybar();
void texth2(const char* title);
void vertical(fnevent proc);
void fronts();
namespace utg {
void beforemodal();
void paint();
int run(fnevent proc, bool darkmode);
void tips();
}
}
typedef flagable<4> flaga;
namespace logs {
extern const char* url;
extern const char* header;
extern bool interactive;
extern flagable<4> multiply_choose;
void apply(const answers& source, const char* title, fncommand proc, int count);
const char* getchoose(const char* id);
int choose(const answers& source, const char* title, const char* cancel);
int choose(const array& source, const char* title, const char* cancel);
int choose(const array& source, const flaga& mask, const char* title, const char* cancel);
}