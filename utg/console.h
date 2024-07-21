#pragma once

void act(const char* format, ...);
void actn(const char* format, ...);
void add_console(char sym, const char* format, const char* format_param);
void add_console(const char* wrap, const char* format, const char* format_param);
void add_console(const char* format, const char* format_param);
void clear_console();
void information(const char* format, ...);
void initialize_console();
void output(const char* format, ...);
void warning(const char* format, ...);