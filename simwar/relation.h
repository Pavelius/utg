#pragma once

struct playeri;

const int player_maximum = 16;
const int relation_maximum = player_maximum * (player_maximum - 1) / 2;

extern int basic_relation[relation_maximum];
extern int bonus_relation[relation_maximum];
extern int current_relation[relation_maximum];

int relation_index(int s1, int s2);
int relation_index(const playeri* p1, const playeri* p2);

void initialize_relation();
void update_relation();