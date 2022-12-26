#pragma once

enum action_s : unsigned char {
	Attack, Defend, Feint, Maneuver
};
enum maneuver_s : unsigned char {
	Impende, GainPosition, Disarm,
};
enum action_roll_s : unsigned char {
	NoRoll, IndependedRoll, VersusRoll,
};
typedef char actiona[Maneuver + 1];
struct actioni {
	const char*		id;
	action_roll_s	rolls[Maneuver + 1];
};
struct maneuveri {
	const char*		id;
	int				cost;
};