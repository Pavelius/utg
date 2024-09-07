#include "draw.h"

static struct tga_bitmap_plugin : public draw::surface::plugin {

#pragma pack(push)
#pragma pack(1)
	struct header {
		char		idlength;
		char		colourmaptype;
		char		datatypecode;
		short int	colourmaporigin;
		short int	colourmaplength;
		char		colourmapdepth;
		short int	x_origin;
		short int	y_origin;
		short		width;
		short		height;
		char		bitsperpixel;
		char		imagedescriptor;
	};
#pragma pack(pop)

	tga_bitmap_plugin() : plugin("tga", "TGA images\0*.tga\0") {
	}

	color read(const unsigned char* ps, char bytes) const {
		color e;
		if(bytes == 4) {
			e.r = ps[0];
			e.g = ps[1];
			e.b = ps[2];
			e.a = ps[3];
		} else if(bytes == 3) {
			e.r = ps[0];
			e.g = ps[1];
			e.b = ps[2];
			e.a = 0;
		} else if(bytes == 2) {
			e.b = (ps[1] & 0x7c) << 1;
			e.g = ((ps[1] & 0x03) << 6) | ((ps[0] & 0xe0) >> 2);
			e.r = (ps[0] & 0x1f) << 3;
			e.a = (ps[1] & 0x80);
		} else
			e.clear();
		return e;
	}

	bool decode(unsigned char* output, int output_bpp, const unsigned char* input, unsigned input_size) override {
		int image_width, image_height, bpp;
		if(!output)
			return false;
		if(!inspect(image_width, image_height, bpp, input, input_size))
			return false;
		auto p = (header*)input;
		auto total_pixels = p->width * p->height;
		auto ps = input + (p->idlength + p->colourmaptype * p->colourmaplength) + sizeof(header);
		auto bytes = p->bitsperpixel / 8;
		auto output_scanline = color_scanline(image_width, output_bpp);
		auto n = 0;
		if(p->datatypecode == 10) {
			while(n < total_pixels) {
				unsigned char b = *ps++;
				if(b & 0x80) {
					auto e = read(ps, bytes); ps += bytes;
					auto count = (b & 0x7F) + 1;
					for(auto i = 0; i < count; i++) {
						auto y = n / image_width;
						auto x = n % image_width;
						e.write((char*)output + output_scanline * y, x, output_bpp);
						n++;
					}
				} else {
					auto count = b + 1;
					for(auto i = 0; i < count; i++) {
						color e = read(ps, bytes); ps += bytes;
						auto y = n / image_width;
						auto x = n % image_width;
						e.write((char*)output + output_scanline * y, x, output_bpp);
						n++;
					}
				}
			}
		} else {
			while(n < total_pixels) {
				color e = read(ps, bytes);
				auto y = n / image_width;
				auto x = n % image_width;
				e.write((char*)output + output_scanline * y, x, output_bpp);
				ps += bytes;
				n++;
			}
		}
		return true;
	}

	bool inspect(int& w, int& h, int& bpp, const unsigned char* input, unsigned size) override {
		auto p = (header*)input;
		if(p->datatypecode != 2 && p->datatypecode != 10)
			return false;
		if(p->colourmaptype != 0)
			return false;
		if(p->bitsperpixel != 32 && p->bitsperpixel != 24)
			return false;
		w = p->width;
		h = p->height;
		bpp = p->bitsperpixel;
		return true;
	}

} instance;

void initialize_tga() {
}