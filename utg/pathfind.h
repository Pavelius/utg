#pragma once

namespace pathfind {
typedef unsigned short indext;
const indext Blocked = 0xFFFF;
const indext StopMovementThrought = 0xFFFE;
typedef indext(*fnto)(indext index, int direction);
extern int		maxcount;
void			blocknearest(indext index, indext cost);
void			blockrange(int range);
void			blockzero();
void			clearpath();
bool			isinitializated();
indext			getfarest(const indext* source);
indext			getnearest(const indext* source);
indext			getmove(indext i);
void			getmove(indext* destination);
void			makewave(indext start_index);
void			setmove(const indext* source);
void			setmove(indext i, indext v);
extern fnto		to;
}