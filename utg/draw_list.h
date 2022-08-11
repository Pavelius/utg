#pragma once

struct sprite;

namespace metrics {
extern sprite*	list;
}
namespace draw {
void			list(int& origin, int& current, int perline, fnevent prow);
void			partoffset(int x, int y);
void			showactive();
void			showbackground();
void			showborder();
}
