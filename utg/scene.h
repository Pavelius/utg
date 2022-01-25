#pragma once

namespace draw {
struct scenei {
	void			background(const char* id, const char* folder = 0) const;
	void			hexagon(short unsigned index) const;
	virtual void	render() {}
	void*			show();
};
}
