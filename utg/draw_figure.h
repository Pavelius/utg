#pragma once

enum class figure : unsigned char {
	None, Check, Circle, CircleFill, Close, Cross,
	Hexagon, FHexagon, HexagonFill,
	Rect, Rect3D, RectFill, Trianlge, TrianlgeUp,
};
namespace draw {
extern figure	hilite_type;
void			field(figure type, int size);
void			field(figure type, int size, const char* format);
}