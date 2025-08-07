#pragma once

typedef void(*fnevent)();

int board_game_run(fnevent proc, const char* rules_url = 0);

void board_tool_button(const char* id, const char* value);
void board_tool_panel(int dx = 32, int dy = 32);
