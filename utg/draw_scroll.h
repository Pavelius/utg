#pragma once

extern int scroll_size;

void input_scroll(int& origin, int maximum, int row_height);
void input_scroll_horiz(int& origin, int maximum, int row_height);
void paint_scroll(int& origin, int maximum, int row_height);
void paint_scroll_horiz(int& origin, int maximum, int row_height);
