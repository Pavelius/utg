#include "answers.h"
#include "crt.h"
#include "draw_figure.h"
#include "flagable.h"
#include "point.h"
#include "variant.h"

#pragma once

namespace draw {
typedef void (*fnstatus)(const void* object, stringbuilder& sb);
extern point offset;
extern const void* hilite_object;
extern figure hilite_type;
extern fnstatus pstatus;
void answerbt(int index, const void* id, const char* title);
void avatar(const char* id, const void* object);
void bar(int value, int maximum);
bool buttonfd(const char* title);
void nextpos();
void noavatar();
void texth2(const char* title);
namespace utg {
void beforemodal();
void paint();
int run(fnevent proc);
void tips();
}
}
typedef flagable<4> flaga;
namespace logs {
extern const char* url;
extern const char* header;
extern bool interactive;
const char* getchoose(const char* id);
int choosei(const array& source, const char* title, const char* cancel);
int choosei(const array& source, const flaga& mask, const char* title, const char* cancel);
}