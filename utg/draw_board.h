#pragma once

typedef void(*fnevent)();

int board_game_run(fnevent proc, const char* rules_url = 0);

void board_tool_button(const char* id, void* object, const char* value);
void board_tool_panel(int dy = 32);
