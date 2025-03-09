#include "editor.h"
#include "io_stream.h"

using namespace draw;

color pallette[256];
vector<surface> frames;
surface bitdata(32, 32, 8);
surface bitcopy(32, 32, 8);
point sprite_size;
static int current_frame;

static void read_pallette_rgb(io::stream& file) {
	for(auto i = 0; i < 256; i++) {
		file.read(pallette[i].r);
		file.read(pallette[i].g);
		file.read(pallette[i].b);
		pallette[i].r <<= 2;
		pallette[i].g <<= 2;
		pallette[i].b <<= 2;
		pallette[i].a = 0;
	}
}

void read_pallette(const char* url) {
	io::file file(url, StreamRead);
	if(!file)
		return;
	read_pallette_rgb(file);
}

static const unsigned char* line_rle8_8(const unsigned char* s, unsigned char* d, int sx) {
	while(true) {
		unsigned char c = *s++;
		if(c == 0)
			return s;
		else if(c <= 0x7F) {
			memcpy(d, s, c);
			s += c;
		} else if(c == 0x80) {
			d += *s++; // Shadow
		} else if(c <= 0x9F) {
			d += c - 0x80; // Shadow
		} else if(c == 0xA0)
			d += (*s++); // Skip count bytes
		else
			d += (c - 0xA0); // Skip N bytes
	}
}

static const unsigned char* line_raw8_8(const unsigned char* s, unsigned char* d, int sx) {
	memcpy(d, s, sx);
	return s + sx;
}

static const unsigned char* line_alc4_8(const unsigned char* s, unsigned char* d, int sx) {
	for(auto x = 0; x < sx; x++) {
		if(x & 1)
			d[x] = s[x / 2] >> 4;
		else
			d[x] = s[x / 2] & 0xF;
	}
	return s + (sx + 1) / 2;
}

static void read_frame(surface& e, const sprite* ps, const sprite::frame& fr) {
	e.resize(fr.sx, fr.sy, 8, true);
	memset(e.ptr(0, 0), 0, e.scanline * e.height);
	auto s = ps->ptr(fr.offset);
	for(auto y = 0; y < fr.sy; y++) {
		auto d = e.ptr(0, y);
		switch(fr.encode) {
		case sprite::RAW8: s = line_raw8_8(s, d, fr.sx); break;
		case sprite::RLE8: s = line_rle8_8(s, d, fr.sx); break;
		case sprite::ALC41: s = line_alc4_8(s, d, fr.sx); break;
		case sprite::ALC8: s = line_alc4_8(s, d, fr.sx); break;
		}
	}
	if(fr.pallette)
		memcpy(pallette, ps->ptr(fr.pallette), 256 * sizeof(color));
}

void read_frames(const char* url) {
	auto ps = (sprite*)loadb(url);
	if(!ps)
		return;
	sprite_size.x = ps->width;
	sprite_size.y = ps->height;
	frames.clear();
	for(auto i = 0; i < ps->count; i++) {
		auto p = frames.add();
		auto& fr = ps->get(i);
		read_frame(*p, ps, fr);
	}
	load_frame(0);
}

void load_frame(int index) {
	current_frame = index;
	if(current_frame >= (int)frames.count)
		current_frame = frames.count - 1;
	if(current_frame < 0)
		current_frame = 0;
	auto& e = frames[current_frame];
	bitdata.resize(e.width, e.height, e.bpp, true);
	memcpy(bitdata.ptr(0, 0), e.ptr(0, 0), e.scanline * e.height);
}

static void cbsetframe() {
	load_frame(hparam);
}

void input_editor() {
	switch(hkey) {
	case '+': execute(cbsetframe, current_frame + 1); break;
	case '-': execute(cbsetframe, current_frame - 1); break;
	}
}

void paint_frames() {
	char temp[260]; stringbuilder sb(temp);
	sb.add("Frames %1i/%2i", current_frame, frames.count);
	texta(temp, AlignCenterCenter);
}