#include "crt.h"
#include "draw.h"
#include "draw_object.h"
#include "screenshoot.h"

using namespace draw;

BSDATAC(object, 256)
BSDATAC(draworder, 256)

static rect last_screen;
long distance(point from, point to);
object object::def;

void draworder::clear() {
	memset(this, 0, sizeof(*this));
}

static void copy(drawable& e1, drawable& e2) {
	e1 = e2;
}

draworder* object::addorder() {
	auto p = bsdata<draworder>::addz();
	copy(*p, *this);
	copy(p->start, *this);
	p->parent = this;
	return p;
}

void object::initialize() {
	def.fore = colors::text;
	def.x = def.y = -10000;
	def.priority = 64;
	def.size = 32;
	def.alpha = 255;
	def.flags = 0;
	def.set(Visible);
}

void object::clear() {
	*this = def;
}

static void textcn(const char* string) {
	auto push_caret = caret;
	caret.x -= textw(string) / 2;
	text(string);
	caret = push_caret;
}

void object::paintns() const {
	if(shape != figure::None)
		field(shape, size);
	if(resource)
		image(caret.x, caret.y, resource, frame, flags);
	if(proc)
		proc();
	if(string)
		textcn(string);
}

static void raw_beforemodal() {
	caret = {0, 0};
	width = getwidth();
	height = getheight();
	hilite_object = 0;
	hot.cursor = cursor::Arrow;
	hot.hilite.clear();
}

static void blend(screenshoot& source, screenshoot& destination, unsigned milliseconds) {
	if(!milliseconds)
		return;
	auto start = getcputime();
	auto finish = start + milliseconds;
	auto current = start;
	while(ismodal() && current < finish) {
		auto alpha = ((current - start) << 8) / milliseconds;
		source.restore();
		canvas->blend(destination, alpha);
		doredraw();
		waitcputime(1);
		current = getcputime();
	}
}

void draw::splashscreen(unsigned milliseconds) {
	screenshoot push;
	raw_beforemodal();
	paintstart();
	paintobjects();
	screenshoot another;
	blend(push, another, milliseconds);
}

void object::paint() const {
	auto push_fore = draw::fore;
	auto push_alpha = draw::alpha;
	auto push_size = draw::fsize;
	draw::fsize = size;
	draw::fore = fore;
	draw::alpha = alpha;
	if(data) {
		auto w = size;
		if(shape == figure::Hexagon || shape == figure::FHexagon)
			w = 2 * w / 3;
		if(ishilite(w)) {
			hilite_object = data;
			hilite_type = figure::None;
			if(is(Hilite) && hilite_object) {
				hilite_type = figure::Circle;
				if(hot.key == MouseLeft && !hot.pressed)
					execute(buttonparam, (long)hilite_object, 0, 0);
			}
		}
	}
	paintns();
	draw::fsize = push_size;
	draw::alpha = push_alpha;
	draw::fore = push_fore;
}

static size_t getobjects(object** pb, object** pe) {
	auto ps = pb;
	for(auto& e : bsdata<object>()) {
		if(ps < pe)
			*ps++ = &e;
	}
	return ps - pb;
}

static int compare(const void* v1, const void* v2) {
	auto p1 = *((object**)v1);
	auto p2 = *((object**)v2);
	if(p1->priority != p2->priority)
		return p1->priority - p2->priority;
	if(p1->y != p2->y)
		return p1->y - p2->y;
	return p1->x - p2->x;
}

static void sortobjects(object** pb, size_t count) {
	qsort(pb, count, sizeof(pb[0]), compare);
}

void draw::paintobjects() {
	auto push_caret = caret;
	auto push_clip = clipping;
	last_screen = {caret.x, caret.y, caret.x + width, caret.y + height};
	setclip(last_screen);
	object* source[128];
	auto count = getobjects(source, source + sizeof(source) / sizeof(source[0]));
	sortobjects(source, count);
	for(size_t i = 0; i < count; i++) {
		draw::caret = *source[i] - draw::camera;
		source[i]->paint();
	}
	clipping = push_clip;
	caret = push_caret;
}

void* draw::chooseobject() {
	draw::scene(paintobjects);
	return (void*)getresult();
}

static void paintobjectsshowmode() {
	paintobjects();
	if(hot.key == KeyEscape)
		execute(buttoncancel);
	if(!hot.pressed && hot.key==MouseLeft)
		execute(buttoncancel);
}

void draw::showobjects() {
	draw::scene(paintobjectsshowmode);
}

object*	draw::addobject(int x, int y) {
	auto p = bsdata<object>::add();
	*p = object::def;
	p->x = x; p->y = y;
	return p;
}

object* draw::findobject(const void* p) {
	for(auto& e : bsdata<object>()) {
		if(e.data == p)
			return &e;
	}
	return 0;
}

void draw::clearobjects() {
	bsdata<object>::source.clear();
}

static void correct_camera(point result, int offs) {
	if(last_screen) {
		rect area = {camera.x + offs, camera.y + offs, camera.x + last_screen.width() - offs, camera.y + last_screen.height() - offs};
		if(!result.in(area)) {
			if(result.x < area.x1)
				camera.x -= area.x1 - result.x;
			if(result.y < area.y1)
				camera.y -= area.y1 - result.y;
			if(result.x > area.x2)
				camera.x += result.x - area.x2;
			if(result.y > area.y2)
				camera.y += result.y - area.y2;
		}
	}
}

static void moving(point& result, point goal, int step, int corrent) {
	auto start = result;
	auto maxds = distance(start, goal);
	auto curds = 0;
	while(ismodal() && curds < maxds) {
		result.x = (short)(start.x + (goal.x - start.x) * curds / maxds);
		result.y = (short)(start.y + (goal.y - start.y) * curds / maxds);
		if(corrent)
			correct_camera(result, corrent);
		if(pbackground)
			pbackground();
		paintobjects();
		doredraw();
		waitcputime(1);
		curds += step;
		if(curds > maxds)
			curds = maxds;
	}
	result = goal;
}

void object::move(point goal, int speed, int correct) {
	moving(*this, goal, speed, correct);
}

const sprite* draw::getbackground(const char* name) {
	return gres(name, "art/objects", {}, 0, 0);
}

const sprite* draw::getres(const char* name) {
	return gres(name, "art/objects", {}, -10000, -10000);
}

const sprite* draw::getres(const char* name, const char* folder) {
	return gres(name, folder, {}, -10000, -10000);
}

void draw::setcamera(point v) {
	auto w = last_screen.width();
	if(!w)
		w = getwidth();
	auto h = last_screen.height();
	if(!h)
		h = getheight();
	camera.x = v.x - w/2;
	camera.y = v.y - h/2;
}