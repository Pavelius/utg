#include "crt.h"
#include "point.h"
#include "variant.h"

#pragma once

namespace draw {
void answerbt(int index, const void* id, const char* title);
void avatar(const char* id);
void bar(int value, int maximum);
bool buttonfd(const char* title);
void texth2(const char* title);
namespace utg {
void beforemodal();
void paint();
int run(fnevent proc);
void tips();
}
}