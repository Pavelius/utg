#pragma once

enum wear_s : unsigned char {
	Head, Body, RightHand, LeftHand, RightRing, LeftRing, Legs,
	Backpack, LastBackpack = Backpack + 16,
};
