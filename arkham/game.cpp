#include "main.h"

gamei game;
quests gamei::quest_other;

int gamei::d6() {
	return 1 + rand() % 6;
}