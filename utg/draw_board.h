#pragma once

typedef void(*fnevent)();

extern const char* board_promt;
extern const char* board_console;

const char* getnms(const char* id);

int board_game_run(fnevent proc, const char* rules_url = 0);

void board_button();
void board_button(const char* format, void* object, unsigned key);
void board_tool(const char* id, unsigned key);
void board_tool(const char* id, void* object, const char* value);
void board_tool(const char* id, void* object, int value, const char* format);
void board_tool_panel(int dy = 32);
bool ishilite(const void* object);
bool ishilite(int size, const void* object);
