#pragma once

struct playeri;
struct provincei;

struct moveorder {
	unsigned short	from, to;
	playeri*		player;
	int				count;
	constexpr explicit operator bool() const { return count != 0; }
	void			clear();
	provincei*		getfrom() const;
	provincei*		getto() const;
};
void add_move(const provincei* from, const provincei* to, playeri* player, int count);
void cancel_move(const provincei* to, const playeri* player);
