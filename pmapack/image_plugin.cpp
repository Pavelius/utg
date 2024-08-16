#include "image.h"
#include "crt.h"
#include "draw.h"
#include "log.h"
#include "sprite_util.h"

using namespace draw;

struct variablei {
	const char*			id;
	void*				object;
	const imagea*		scope;
	constexpr operator bool() const { return id != 0; }
};
struct colormapi {
	color				fore;
	int					count;
	unsigned char		index;
};

const char*				util::image_source;
static sprite*			last_sprite;
static surface			bitmaps[16];
static color			pallette[256];
static adat<variablei>	variables;
static vector<colormapi> colormap;

static void error(const char* format, ...) {
	log::errorv(0, format, xva_start(format));
}

static void message(const char* format, ...) {
	log::printv(format, xva_start(format));
	log::println();
}

static bool error_no_sprite() {
	if(!last_sprite) {
		error("Sprite do not exist. You must use `CreateSprite` before any sprite manupulation operations.");
		return true;
	}
	return false;
}

static surface* get_surface(int param) {
	int maximum = sizeof(bitmaps) / sizeof(bitmaps[0]);
	if(param < 0) {
		error("Bitmap param %1i can not be lesser that zero.", param);
		return 0;
	} else if(param >= maximum) {
		error("Bitmap param %1i can not be greater that %2i.", param, maximum);
		return 0;
	}
	return bitmaps + last_image.param;
}

static surface* get_surface() {
	return get_surface(last_image.param);
}

static void remove_alpha() {
	auto ps = get_surface();
	if(!ps)
		return;
	for(auto y = 0; y < ps->height; y++) {
		auto pb = (color*)ps->ptr(0, y);
		auto pe = (color*)ps->ptr(ps->width, y);
		while(pb < pe) {
			pb->a = 0;
			pb++;
		}
	}
}

static int compare_color(const void* v1, const void* v2) {
	auto p1 = (color*)v1;
	auto p2 = (color*)v2;
	if(p1->r != p2->r)
		return p1->r - p2->r;
	if(p1->g != p2->g)
		return p1->g - p2->g;
	if(p1->b != p2->b)
		return p1->b - p2->b;
	return 0;
}

static int compare_colormap(const void* v1, const void* v2) {
	auto p1 = (colormapi*)v1;
	auto p2 = (colormapi*)v2;
	if(p1->count != p2->count)
		return p2->count - p1->count;
	return 0;
}

static void create_sprite() {
	if(last_sprite) {
		error("Sprite already created another `CreateSprite` operations. You must use `SaveSprite` operations before creating any other sprites.");
		return;
	}
	last_sprite = (sprite*)new char[256 * 256 * 64];
	sprite_create(last_sprite, last_image.param, 0, 0);
}

static colormapi* find_colormap(color v) {
	for(auto& e : colormap) {
		if(e.fore == v)
			return &e;
	}
	return 0;
}

static void create_color_map() {
	colormap.clear();
	auto ps = get_surface();
	if(!ps)
		return;
	auto total = ps->width * ps->height;
	auto pcol = new color[total];
	auto p = pcol;
	for(auto y = 0; y < ps->height; y++) {
		auto pb = (color*)ps->ptr(0, y);
		for(auto x = 0; x < ps->width; x++) {
			*p = pb[x];
			p->a = 0;
			p++;
		}
	}
	qsort(pcol, total, sizeof(p[0]), compare_color);
	auto last = colormap.add();
	last->fore = pcol[0];
	last->count = 0;
	last->index = 0;
	auto pb = pcol;
	for(auto pe = pcol + total; pb < pe; pb++) {
		if(last->fore == *pb)
			last->count++;
		else {
			last = colormap.add();
			last->fore = *pb;
			last->count = 1;
			last->index = 0;
		}
	}
	delete[] pcol;
	qsort(colormap.data, colormap.count, colormap.size, compare_colormap);
}

static void create_grayscale_pallette() {
	for(auto i = 0; i < 256; i++) {
		pallette[i].r = i;
		pallette[i].g = i;
		pallette[i].b = i;
		pallette[i].a = 0;
	}
}

static void create_compatible(surface* p1, const surface* p2) {
	p1->clear();
	p1->resize(p2->width, p2->height, p2->bpp, true);
}

static void copy_image() {
	auto p1 = get_surface(0);
	if(!p1)
		return;
	auto p2 = get_surface();
	if(!p2)
		return;
	create_compatible(p1, p2);
	blit(*p1, 0, 0, p1->width, p1->width, 0, *p2, 0, 0);
}

static void load_image() {
	auto ps = get_surface();
	if(!ps)
		return;
	auto p = image_source_url();
	if(!ps->read(p, 0, last_image.bpp)) {
		error("Can't find image `%1`", p);
		return;
	}
}

static void save_image() {
	auto ps = get_surface();
	if(!ps)
		return;
	auto p = image_dest_url("bmp");
	ps->write(p, pallette);
}

static void default_sprite_coordinates(surface* ps) {

	if(last_image.size.x == -1)
		last_image.size.x = ps->width;
	if(last_image.size.y == -1)
		last_image.size.y = ps->height;

	if(last_image.center.x == -1)
		last_image.center.x = last_image.size.x / 2;
	if(last_image.center.y == -1)
		last_image.center.y = last_image.size.y / 2;

}

static void add_frame() {

	if(error_no_sprite())
		return;
	auto ps = get_surface();
	if(!ps)
		return;

	default_sprite_coordinates(ps);

	sprite_store(last_sprite,
		ps->ptr(last_image.position.x, last_image.position.y),
		ps->scanline, last_image.size.x, last_image.size.y,
		last_image.center.x, last_image.center.y,
		sprite::Auto, 0, 0, -1, 0);

}

static void save_sprite() {
	if(error_no_sprite())
		return;
	auto p = image_dest_url();
	message("Creating `%1`", p);
	sprite_write(p, last_sprite);
	delete last_sprite;
	last_sprite = 0;
}

static void help_info() {
	message("List of possible command you can use:");
	for(auto& e : bsdata<imageplugini>()) {
		auto pd = e.description;
		if(!pd)
			continue;
		message(" %1 - %2", e.id, pd);
	}
}

void initialize_image_plugins() {
}

BSDATA(imageplugini) = {
	{"AddFrame", add_frame, "Add frame to sprite from bitmap. Use [position], [size], [center] etc."},
	{"CreateColormap", create_color_map},
	{"CreateGrayscalePallette", create_grayscale_pallette},
	{"CreateSprite", create_sprite, "Create sprite with [param] number of frames."},
	{"CopyImage", copy_image, "Copy bitmap with index 0 to bitmap with index [param]."},
	{"Help", help_info, "Show this information."},
	{"LoadImage", load_image, "Load bitmap from disk by source path. Use [name] as it name."},
	{"SaveImage", save_image, "Save bitmap to disk by destination path. Use [name] as it name."},
	{"SaveSprite", save_sprite, "Save sprite to disk by destination path. Use [name] as it name."},
};
BSDATAF(imageplugini)