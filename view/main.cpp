﻿#include "draw.h"
#include "io_stream.h"
#include "stringbuilder.h"

using namespace draw;

void set_light_theme();

namespace setting {
namespace show {
bool offset;
bool center = true;
bool grid;
bool mirror;
bool vmirror;
namespace text {
bool bottom;
}
}
}

static void enter_number(int& value) {
	char temp[32]; stringbuilder sb(temp);
	//rect rc = {getwidth() - 100, 4, getwidth() - 4, 4 + texth() + 8};
	sb.add("%1i", value);
	//controls::textedit te(temp, sizeof(temp), true);
	//if(!te.editing(rc))
	//	return;
	psnum(temp, value);
}

char* szpne(char* result, const char* url, const char* name, const char* ext) {
	char temp[260]; stringbuilder s1(temp); s1.add(url);
	stringbuilder sb(result, result + 260);
	char* p1 = (char*)szfname(temp);
	if(p1)
		p1[0] = 0;
	sb.clear();
	sb.add(temp);
	sb.add(name);
	sb.add(".");
	sb.add(ext);
	return result;
}

unsigned char* load_pallette(const char* url) {
	char temp[260];
	char temp2[260];
	void* result;
	result = loadb(szpne(temp, url, "main", "pal"));
	if(!result)
		result = loadb(szpne(temp, url, szfnamewe(temp2, url), "pal"));
	return (unsigned char*)result;
}

static const char* mode_names[] = {"Auto", "RAW", "RLE", "ALC", "RAW8", "RLE8", "ALC8", "RAW1"};
static_assert(sizeof(mode_names) / sizeof(mode_names[0]) == (sprite::RAW1 + 1), "Invalid mode name descriptions");

static void show_grid_lines() {
	auto push_caret = caret;
	auto push_fore = fore;
	const int grid_size = 10;
	const color grid_color = colors::form.darken();
	const color grid_bold = colors::form.darken().darken();
	for(int x = 0; x < width; x += grid_size) {
		fore = (x % (grid_size * 10)) == 0 ? grid_bold : grid_color;
		caret.x = x;
		caret.y = 0;
		line(caret.x, height);
	}
	for(int y = 0; y < height; y += grid_size) {
		fore = (y % (grid_size * 10)) == 0 ? grid_bold : grid_color;
		caret.x = 0;
		caret.y = y;
		line(width, caret.y);
	}
	fore = push_fore;
	caret = push_caret;
}

static void show_marker(int sx, int sy) {
	auto push_caret = caret;
	auto push_fore = fore;
	fore = colors::red;
	line(caret.x, caret.y - sy);
	caret = push_caret;
	line(caret.x - sx, caret.y);
	fore = push_fore;
	caret = push_caret;
}

void dlgmsg(const char* title, const char* format);

void mainview(const char* url) {
	unsigned char* pal = 0;
	sprite* pi = (sprite*)loadb(url);
	if(!pi) {
		char temp[512]; stringbuilder sb(temp);
		sb.add("File not found:\n\n%1", url);
		//dlgmsg("X-view", temp);
		return;
	}
	int wx = 640;
	int wy = 480;
	const sprite::frame& f1 = pi->get(0);
	bool background_image = (f1.encode == sprite::RAW || f1.encode == sprite::RAW8) && f1.ox == 0 && f1.oy == 0;
	if(background_image) {
		wx = f1.sx;
		wy = f1.sy;
		setting::show::center = false;
	}
	draw::initialize(0);
	draw::palt = (color*)load_pallette(url);
	settimer(100);
	setcaption(szfname(url));
	int current = 0;
	int tick = 0;
	char temp[512]; stringbuilder sb(temp);
	bool animated_sprite = pi->cicles != 0;
	while(ismodal()) {
		fillform();
		int maximum = animated_sprite ? pi->cicles : pi->count;
		if(current >= maximum)
			current = maximum - 1;
		if(current < 0)
			current = 0;
		if(setting::show::grid)
			show_grid_lines();
		int x = width / 2;
		int y = height / 2;
		int current_frame = animated_sprite ? pi->ganim(current, tick) : current;
		const sprite::frame& f = pi->get(current_frame);
		int sx = f.ox;
		int sy = f.oy;
		int cx = f.sx;
		int cy = f.sy;
		unsigned flags_image = 0;
		if(setting::show::mirror)
			flags_image |= ImageMirrorH;
		if(setting::show::vmirror)
			flags_image |= ImageMirrorV;
		if(setting::show::center)
			image(x, y, pi, current_frame, flags_image);
		else
			image(0, 0, pi, current_frame, ImageNoOffset | flags_image);
		if(setting::show::offset) {
			auto push_caret = caret;
			caret.x = x;
			caret.y = y;
			show_marker(sx, sy);
			caret = push_caret;
		}
		sb.clear();
		sb.add("[%2i/%3i](%4i,%5i, size=%6ix%7i)", szfname(url), current, maximum, sx, sy, cx, cy);
		if(animated_sprite)
			sb.addn("frames total %1i", pi->count);
		if(setting::show::center)
			sb.adds("center");
		sb.adds(mode_names[f.encode]);
		auto frame_name = pi->getstring(current);
		if(frame_name && frame_name[0])
			sb.addn(frame_name);
		rect rc = {0, 0, getwidth(), getheight()};
		rc.offset(metrics::padding);
		if(setting::show::text::bottom)
			text(rc, temp, AlignLeftBottom);
		else
			text(rc, temp, AlignLeft);
		domodal();
		switch(hkey) {
		case KeyRight:
			current++;
			break;
		case KeyLeft:
			current--;
			break;
		case 'B':
			setting::show::offset = !setting::show::offset;
			break;
		case 'C':
			setting::show::center = !setting::show::center;
			break;
		case 'M':
			setting::show::mirror = !setting::show::mirror;
			break;
		case 'U':
			setting::show::vmirror = !setting::show::vmirror;
			break;
		case 'G':
			setting::show::grid = !setting::show::grid;
			break;
		case 'F':
			enter_number(current);
			break;
		case 'N':
			setting::show::text::bottom = !setting::show::text::bottom;
			break;
		case InputTimer:
			tick++;
			break;
		default:
			break;
		}
	}
}

const char* sznext(const char* p) {
	if(*p == '\"') {
		p++;
		while(*p != '\"')
			p++;
		p++;
	} else {
		while(*p && (*p != ' ' && *p != '\t' && *p != 10 && *p != 13))
			p++;
	}
	return p;
}

static void correct_font() {
	char furl[260];
	char modl[260];
	if(metrics::font)
		return;
	if(!io::file::getmodule(modl, sizeof(modl) / sizeof(modl[0])))
		return;
	auto pc = (char*)szfname(modl);
	if(pc > modl && pc[-1] == '/' || pc[-1] == '\\')
		pc[-1] = 0;
	else
		pc[0] = 0;
	// dlgmsg("Module", modl);
	static const char* font_directories[] = {modl, ""};
	for(auto temp : font_directories) {
		szurl(furl, temp, "font", "pma");
		metrics::font = (sprite*)loadb(furl);
		if(!metrics::font) {
			// dlgmsg("Not found", furl);
			szurl(furl, temp, "art/font", "pma");
			metrics::font = (sprite*)loadb(furl);
		}
		if(!metrics::font) {
			// dlgmsg("Not found", furl);
			szurl(furl, temp, "art/fonts/font", "pma");
			metrics::font = (sprite*)loadb(furl);
		}
		//if(!metrics::font)
		//	dlgmsg("Not found", furl);
		if(metrics::font)
			break;
	}
}

static void test_directory() {
	char temp[260]; io::file::getdir(temp, sizeof(temp));
	if(temp[0])
		return;
}

int main(int argc, char *argv[]) {
	//test_directory();
	correct_font();
	set_light_theme();
	if(argc > 1 && argv[1][0]) {
		mainview(argv[1]);
	} else {
		//dlgmsg("PMA view", "Show sprites in PMA format. Pass to command line path to file wich you want to see.");
		return -1;
	}
	return 0;
}

char** szcmdargv(int& argc);

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	int argc;
	char** argv = szcmdargv(argc);
	return main(argc, argv);
}