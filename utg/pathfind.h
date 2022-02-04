#pragma once

namespace pathfind {
typedef unsigned short indext;
const indext Blocked = 0xFFFF;
const indext StopMovementThrought = 0xFFFE;
typedef indext(*fnto)(indext index, int direction);
extern int		maxcount;
extern int		maxdir;
extern fnto		to;
void			blocknearest(indext index, indext cost);
void			blockrange(int range);
void			blockzero();
void			clearpath();
bool			isinitializated();
indext			getfarest(const indext* source);
indext			getnearest(const indext* source);
indext			getmove(indext i);
void			getmove(indext* destination);
unsigned		getpath(indext start, indext goal, indext* result, unsigned maximum);
void			makewave(indext start_index);
void			setmove(const indext* source);
void			setmove(indext i, indext v);
}