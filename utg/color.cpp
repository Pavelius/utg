#include "color.h"
#include "math.h"
#include "slice.h"

color colors::black;
color colors::blue(0, 0, 255);
color colors::gray(128, 128, 128);
color colors::green(0, 255, 0);
color colors::red(255, 0, 0);
color colors::yellow(255, 244, 32);
color colors::white(255, 255, 255);

color color::gray() const {
	unsigned char level = (r + g + b) / 3;
	return{level, level, level, 0};
}

bool color::isdark() const {
	return gray().r < 100;
}

color color::lighten() const {
	int		n;
	color	result;
	n = r + (r >> 2);
	if(n > 255)
		result.r = 255;
	else
		result.r = n;
	n = g + (g >> 2);
	if(n > 255)
		result.g = 255;
	else
		result.g = n;
	n = b + (b >> 2);
	if(n > 255)
		result.b = 255;
	else
		result.b = n;
	return result;
}

color color::darken() const {
	color result;
	result.r = r - (r >> 3);
	result.g = g - (g >> 3);
	result.b = b - (b >> 3);
	return result;
}

color color::mix(const color c1, unsigned char a) const {
	color result;
	result.r = (((int)r * a) + (c1.r * (255 - a))) >> 8;
	result.g = (((int)g * a) + (c1.g * (255 - a))) >> 8;
	result.b = (((int)b * a) + (c1.b * (255 - a))) >> 8;
	return result;
}

color color::negative() const {
	color result;
	result.r = ~r;
	result.g = ~g;
	result.b = ~b;
	return result;
}

int color::find(const void* pallette, int count) const {
	color* p = (color*)pallette;
	int result = 0;
	int mcr = 1000;
	int mcg = 1000;
	int mcb = 1000;
	for(int i = 0; i < count; i++) {
		color& m = p[i];
		int xcr = iabs(m.r - r);
		int xcg = iabs(m.g - g);
		int xcb = iabs(m.b - b);
		if(xcr <= mcr && xcg <= mcg && xcb <= mcb) {
			result = i;
			mcr = xcr;
			mcg = xcg;
			mcb = xcb;
		}
	}
	return result;
}

int	color_scanline(int width, int bpp) {
	switch(iabs(bpp)) {
	case 1:
		return ((width + 31) / 32) * 4;
	case 2:
		return ((width + 15) / 16) * 4;
	case 4:
		return ((width + 7) / 8) * 4;
	case 8:
		return ((width + 3) / 4) * 4;
	case 16:
	case -16:
		return ((width * 2 + 3) / 4) * 4;
	case 24:
	case -24:
		return ((width * 3 + 3) / 4) * 4;
	case 32:
	case -32:
		return width * 4;
	default:
		return 0;
	}
}

void color_rgb2bgr(color* source, int count) {
	for(int i = 0; i < count; i++)
		iswap(source[i].r, source[i].b);
}

void color::read(const void* p1, int x, int bpp, const void* pallette) {
	unsigned char* p;
	switch(bpp) {
	case 1:
		r = g = b = a = 0;
		break;
	case 2:
		r = g = b = a = 0;
		break;
	case 4:
		a = 0;
		break;
	case -8:
		p = (unsigned char*)p1 + x;
		p = (unsigned char*)&((unsigned char*)pallette)[(*p) * 4];
		b = p[0];
		g = p[1];
		r = p[2];
		a = 255;
		break;
	case 8:
		p = (unsigned char*)p1 + x;
		p = (unsigned char*)&((unsigned char*)pallette)[(*p) * 4];
		r = p[0];
		g = p[1];
		b = p[2];
		a = 255;
		break;
	case -16:
		p = (unsigned char*)p1 + x * 2;
		r = g = b = p[0];
		a = p[1];
		break;
	case 16:
		p = (unsigned char*)p1 + x * 2;
		b = (p[0] & 0xF) << 4;
		g = ((p[0] >> 4) & 0xF) << 4;
		r = (p[1] & 0xF) << 4;
		a = ((p[1] >> 4) & 0xF) << 4;
		break;
	case -24:
		p = (unsigned char*)p1 + x * 3;
		r = p[2];
		g = p[1];
		b = p[0];
		a = 255;
		break;
	case 24:
		p = (unsigned char*)p1 + x * 3;
		r = p[0];
		g = p[1];
		b = p[2];
		a = 255;
		break;
	case -32:
		p = (unsigned char*)p1 + x * 4;
		r = p[2];
		g = p[1];
		b = p[0];
		a = p[3];
		break;
	case 32:
		p = (unsigned char*)p1 + x * 4;
		r = p[0];
		g = p[1];
		b = p[2];
		a = p[3];
		break;
	}
}

void color::write(void* p1, int x, int bpp, const void* pallette, int color_count) const {
	unsigned char* p;
	switch(bpp) {
	case 1:
		break;
	case 2:
		break;
	case 4:
		if(color_count == -1)
			color_count = 16;
		break;
	case 8:
		if(color_count == -1)
			color_count = 256;
		*((unsigned char*)p1 + x) = find(pallette, color_count);
		break;
	case -32:
		((color*)p1)[x].r = r;
		((color*)p1)[x].g = g;
		((color*)p1)[x].b = b;
		((color*)p1)[x].a = a;
		break;
	case 32:
		((color*)p1)[x].r = b;
		((color*)p1)[x].g = g;
		((color*)p1)[x].b = r;
		((color*)p1)[x].a = a;
		break;
	case -24:
		p = (unsigned char*)p1 + x * 3;
		p[0] = r;
		p[1] = g;
		p[2] = b;
		break;
	case 24:
		p = (unsigned char*)p1 + x * 3;
		p[0] = b;
		p[1] = g;
		p[2] = r;
		break;
	}
}

void color_convert(void* output, int width, int height, int output_bpp, const void* output_pallette, const void* input, int input_bpp, const void* input_pallette, int input_scanline) {
	if(output_bpp == input_bpp)
		return;
	int isc = (input_scanline == 0) ? color_scanline(width, input_bpp) : input_scanline;
	int osc = color_scanline(width, output_bpp);
	const void* input_static = 0;
	auto abs_output_bpp = iabs(output_bpp);
	auto abs_input_bpp = iabs(input_bpp);
	if(abs_output_bpp > abs_input_bpp && input == output) {
		input_static = input;
		auto old_input = input;
		input = new char[isc * height];
		memcpy((void*)input, old_input, isc * height);
	}
	unsigned char* ip = (unsigned char*)input;
	unsigned char* op = (unsigned char*)output;
	color e;
	while(height-- > 0) {
		for(int x = 0; x < width; x++) {
			e.read(ip, x, input_bpp, input_pallette);
			e.write(op, x, output_bpp, output_pallette);
		}
		op += osc;
		ip += isc;
	}
	if(input_static)
		delete[] (char*)input;
}

void color_flipv(unsigned char* bits, unsigned scanline, int height) {
	if(height < 2)
		return;
	auto p1 = bits;
	auto p2 = bits + scanline * (height - 1);
	while(p1 < p2) {
		for(unsigned i = 0; i < scanline; i++)
			iswap(p1[i], p2[i]);
		p1 += scanline;
		p2 -= scanline;
	}
}