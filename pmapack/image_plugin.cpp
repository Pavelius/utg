#include "image.h"
#include "crt.h"
#include "draw.h"
#include "sprite_util.h"

using namespace draw;

const char*		util::image_source;
static sprite*	last_sprite;
static surface	bitmap;

static void error(const char* format, ...) {
	if(!image_errorv_proc)
		return;
	image_errorv_proc(format, xva_start(format));
}

static void message(const char* format, ...) {
	if(!image_messagev_proc)
		return;
	image_messagev_proc(format, xva_start(format));
}

static void create_sprite() {
	if(last_sprite) {
		error("Sprite already created another `CreateSprite` operations. You must use `SaveSprite` operations before creating any other sprites.");
		return;
	}
	last_sprite = (sprite*)new char[256 * 256 * 64];
	sprite_create(last_sprite, last_image.param, 0, 0);
}

static void load_image() {
	auto p = image_source_url();
	if(!bitmap.read(p, 0, last_image.bpp)) {
		error("Can't find image `%1`", p);
		return;
	}
}

static bool error_no_sprite() {
	if(!last_sprite) {
		error("Sprite do not exist. You must use `CreateSprite` before any sprite manupulation operations.");
		return true;
	}
	return false;
}

static void add_frame() {
	
	if(error_no_sprite())
		return;

	if(last_image.size.x == DefaultImageNumber)
		last_image.size.x = bitmap.width;
	if(last_image.size.y == DefaultImageNumber)
		last_image.size.y = bitmap.height;

	if(last_image.center.x == DefaultImageNumber)
		last_image.center.x = last_image.size.x / 2;
	if(last_image.center.y == DefaultImageNumber)
		last_image.center.y = last_image.size.y / 2;

	sprite_store(last_sprite,
		bitmap.ptr(last_image.position.x, last_image.position.y),
		bitmap.scanline, last_image.size.x, last_image.size.y,
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

void initialize_image_plugins() {
}

BSDATA(imageplugini) = {
	{"AddFrame", add_frame},
	{"CreateSprite", create_sprite},
	{"LoadImage", load_image},
	{"SaveSprite", save_sprite},
};
BSDATAF(imageplugini)