#include "bsreq.h"
#include "crt.h"
#include "draw.h"
#include "drawable.h"
#include "screenshoot.h"
#include "variant.h"

using namespace draw;

drawable::fnprepare drawable::preparing;
drawable::fnpaint drawable::painting;

static adat<drawable*, 512> objects;

unsigned long drawable_stamp;
static unsigned long drawable_stamp_last;
static rect last_screen;
static point camera_drag;

long distance(point p1, point p2);

static void correct_camera(point result, int offs) {
	if(last_screen) {
		rect area = drawable::getscreen(offs);
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

//static void remove_orders() {
//	auto pb = bsdata<orderable>::begin();
//	auto pe = bsdata<orderable>::end();
//	while(pe > pb) {
//		pe--;
//		if(*pe)
//			break;
//		bsdata<orderable>::source.count--;
//	}
//}

//static void update_all_orders() {
//	for(auto& e : bsdata<orderable>()) {
//		if(e)
//			e.domove();
//	}
//}

static void start_timer() {
	drawable_stamp_last = getcputime();
}

static void update_timestamp() {
	auto c = getcputime();
	if(!drawable_stamp_last || c < drawable_stamp_last)
		drawable_stamp_last = c;
	auto d = c - drawable_stamp_last;
	if(d < 1000)
		drawable_stamp += d;
	drawable_stamp_last = c;
}

void drawable::addobject() {
	objects.add(this);
}

static void prepare_objects() {
	if(drawable::preparing)
		drawable::preparing();
}

static int compare(const void* v1, const void* v2) {
	auto p1 = *((drawable**)v1);
	auto p2 = *((drawable**)v2);
	auto a1 = p1->priority / 10;
	auto a2 = p2->priority / 10;
	if(a1 != a2)
		return a1 - a2;
	if(p1->position.y != p2->position.y)
		return p1->position.y - p2->position.y;
	if(p1->position.x != p2->position.x)
		return p1->position.x - p2->position.x;
	return a1 - a2;
}

static void sortobjects(drawable** pb, size_t count) {
	qsort(pb, count, sizeof(pb[0]), compare);
}

void drawable::clear() {
	memset(this, 0, sizeof(*this));
}

void drawable::paint() const {
	if(painting)
		painting(this);
}

void drawable::paintall() {
	auto push_caret = caret;
	auto push_clip = clipping;
	last_screen = {caret.x, caret.y, caret.x + width, caret.y + height};
	setclip(last_screen);
	objects.clear();
	prepare_objects();
	sortobjects(objects.data, objects.count);
	for(auto p : objects) {
		draw::caret = p->position - camera;
		p->paint();
	}
	clipping = push_clip;
	caret = push_caret;
}

rect drawable::getscreen(int offs) {
	return {camera.x + offs, camera.y + offs,
		camera.x + last_screen.width() - offs,
		camera.y + last_screen.height() - offs};
}

static point getcameraorigin(point v) {
	auto w = last_screen.width();
	if(!w)
		w = getwidth();
	auto h = last_screen.height();
	if(!h)
		h = getheight();
	v.x -= w / 2;
	v.y -= h / 2;
	return v;
}

void setcamera(point v) {
	camera = getcameraorigin(v);
}

void drawable::input_camera() {
	const int step = 32;
	if(!hot.mouse.in(last_screen))
		return;
	switch(hot.key) {
	case KeyLeft: execute(cbsetsht, camera.x - step, 0, &camera.x); break;
	case KeyRight: execute(cbsetsht, camera.x + step, 0, &camera.x); break;
	case KeyUp: execute(cbsetsht, camera.y - step, 0, &camera.y); break;
	case KeyDown: execute(cbsetsht, camera.y + step, 0, &camera.y); break;
	case MouseWheelUp: execute(cbsetsht, camera.y - step, 0, &camera.y); break;
	case MouseWheelDown: execute(cbsetsht, camera.y + step, 0, &camera.y); break;
	case MouseRight:
		if(hot.pressed && !hot.hilite) {
			dragbegin(&camera);
			camera_drag = camera;
		}
		break;
	default:
		if(dragactive(&camera)) {
			hot.cursor = cursor::All;
			if(hot.mouse.x >= 0 && hot.mouse.y >= 0)
				camera = camera_drag + (dragmouse - hot.mouse);
		}
		break;
	}
}

void drawable::focusing() const {
	const int offset = 16;
	if(!position.in(drawable::getscreen(offset)))
		drawable::slide(position, offset);
}

void drawable::slide(point goal, int step) {
	goal = getcameraorigin(goal);
	auto start = camera;
	auto maxds = distance(start, goal);
	if(!maxds)
		return;
	auto curds = 0;
	while(curds < maxds && ismodal()) {
		curds += step;
		if(curds > maxds)
			curds = maxds;
		camera.x = (short)(start.x + (goal.x - start.x) * curds / maxds);
		camera.y = (short)(start.y + (goal.y - start.y) * curds / maxds);
		paintstart();
		doredraw();
		waitcputime(1);
	}
}

void drawable::dowait() {
	paintstart();
	if(draw::ptips)
		draw::ptips();
	doredraw();
	waitcputime(1);
	update_timestamp();
}

void drawable::splash(unsigned milliseconds) {
	screenshoot push;
	paintstart();
	screenshoot another;
	push.blend(another, milliseconds);
}