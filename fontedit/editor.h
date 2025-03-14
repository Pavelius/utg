#pragma once

#include "color.h"
#include "draw.h"
#include "vector.h"

extern color pallette[256];
extern vector<surface> frames;
extern surface bitdata, bitcopy;
extern point sprite_size;

void input_editor();
void load_frame(int index);
void paint_frames();
void read_pallette(const char* url);
void read_frames(const char* url);