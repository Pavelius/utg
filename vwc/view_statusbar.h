#pragma once

class stringbuilder;

extern stringbuilder status_sb;

void before_modal_statusbar();
void paint_statusbar();
void status(const char* format, ...);
void tips_statusbar();
