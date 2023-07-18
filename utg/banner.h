#include "color.h"
#include "nameable.h"

#pragma once

struct banneri : nameable {
	color		fore, border;
};

void show_banner(int size, int n, const char* format);
void show_banner(int size, int n);
void show_bannerb(int size, int n);
void show_bannerf(int size, int n);