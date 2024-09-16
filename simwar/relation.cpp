#include "bsdata.h"
#include "relation.h"

static int relation_indecies[player_maximum][player_maximum];
int basic_relation[relation_maximum];
int bonus_relation[relation_maximum];
int current_relation[relation_maximum];

void initialize_relation() {
	for(auto y = 0; y < player_maximum; y++) {
		for(auto x = 0; x < player_maximum; x++) {
			relation_indecies[y][x] = -2;
		}
	}
	auto index = 0;
	for(auto y = 0; y < player_maximum; y++) {
		for(auto x = 0; x < player_maximum; x++) {
			if(x == y) {
				relation_indecies[y][x] = -1;
				continue;
			}
			if(relation_indecies[y][x] == -2 && relation_indecies[x][y] != -2) {
				relation_indecies[y][x] = -1;
				continue;
			}
			relation_indecies[y][x] = index++;
		}
	}
}

int relation_index(int s1, int s2) {
	if(s1 < 0 || s2 < 0)
		return -1;
	if(s1 >= player_maximum || s2 >= player_maximum)
		return -1;
	auto i = relation_indecies[s1][s2];
	return (i != -1) ? i : relation_indecies[s2][s1];
}


int relation_index(const playeri* p1, const playeri* p2) {
	return relation_index(
		bsdata<playeri>::source.indexof(p1),
		bsdata<playeri>::source.indexof(p2));
}

void update_relation() {
	for(auto i = 0; i < relation_maximum; i++) {
		if(current_relation[i] > basic_relation[i])
			current_relation[i]--;
		else if(current_relation[i] < basic_relation[i])
			current_relation[i]++;
	}
}