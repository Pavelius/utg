#include "nameable.h"

#pragma once

enum wear_s : unsigned char {
	Backpack, LastBackpack = Backpack + 23,
	Head, Neck, Body, Rear, LeftFinger, RightFinger, Hands, Gridle, Legs,
};
struct weari : nameable {
};