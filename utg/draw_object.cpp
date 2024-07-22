#include "crt.h"
#include "draw.h"
#include "draw_object.h"
#include "screenshoot.h"

using namespace draw;

const size_t max_object_count = 512;

BSDATAC(object, max_object_count)
BSDATAC(draworder, max_object_count)

point objects_mouse;
rect objects_screen;
fnevent object_before_paint;

static unsigned long timestamp, timestamp_last;
static point camera_drag;

long distance(point from, point to);

object* last_object;

static void remove_depends(const draworder* p) {
	for(auto& e : bsdata<draworder>()) {
		if(e.depend == p)
			e.depend = 0;
	}
}

static void remove_dead_objects() {
	auto ps = bsdata<object>::begin();
	for(auto& e : bsdata<object>()) {
		if(!e)
			continue;
		*ps++ = e;
	}
	bsdata<object>::source.count = ps - bsdata<object>::elements;
}

point drawable::getscreen() const {
	return isabsolute() ? position : position - camera;
}

void draworder::clear() {
	memset(this, 0, sizeof(*this));
	remove_depends(this);
}

inline void copy(drawable& e1, drawable& e2) {
	e1 = e2;
}

static void remove_orders() {
	auto pb = bsdata<draworder>::begin();
	auto pe = bsdata<draworder>::end();
	while(pe > pb) {
		pe--;
		if(*pe)
			break;
		bsdata<draworder>::source.count--;
	}
}

static void start_timer() {
	timestamp_last = getcputime();
}

static void update_timestamp() {
	auto c = getcputime();
	if(!timestamp_last || c < timestamp_last)
		timestamp_last = c;
	timestamp += c - timestamp_last;
	timestamp_last = c;
}

static void update_all_orders() {
	for(auto& e : bsdata<draworder>()) {
		if(e)
			e.update();
	}
}

static int calculate(int v1, int v2, int n, int m) {
	return v1 + (v2 - v1) * n / m;
}

void draworder::finish() {
	parent->position = position;
	parent->alpha = alpha;
	if(priority == 250) // Special case - destroy parent object if order is 0
		parent->clear();
	clear();
}

void draworder::update() {
	if(depend || tick_start > timestamp)
		return;
	int m = tick_stop - tick_start;
	if(!m) {
		clear();
		return;
	}
	int n = timestamp - tick_start;
	if(n >= m)
		n = m;
	parent->position.x = (short)calculate(start.position.x, position.x, n, m);
	parent->position.y = (short)calculate(start.position.y, position.y, n, m);
	parent->alpha = (unsigned char)calculate(start.alpha, alpha, n, m);
	if(n == m) {
		if(priority == 250) // Special case - destroy parent object if order is 0
			parent->clear();
		clear();
	}
}

void object::disappear(int milliseconds) {
	auto po = addorder(1000);
	po->alpha = 0;
	po->priority = 0;
	po->autoclear();
}

draworder* object::addorder(int milliseconds, draworder* depend) {
	auto p = bsdata<draworder>::addz();
	if(depend) {
		copy(*p, *depend);
		copy(p->start, *depend);
		p->tick_start = depend->tick_stop;
		p->depend = depend;
	} else {
		copy(*p, *this);
		copy(p->start, *this);
		p->tick_start = timestamp;
	}
	p->parent = this;
	p->tick_stop = p->tick_start + milliseconds;
	return p;
}

void object::clear() {
	painting = 0;
	data = 0;
}

void paint_sprite() {
	image((sprite*)last_object->data, last_object->param, last_object->flags);
}

static void paint_objects_show_mode() {
	paint_objects();
	if(hkey == KeyEscape)
		execute(buttoncancel);
	else if(hkey == MouseLeft && !hpressed)
		execute(buttoncancel);
}

void show_objects() {
	draw::scene(paint_objects_show_mode);
}

void splash_screen(unsigned milliseconds, fnevent proc) {
	if(!proc)
		proc = paintstart;
	auto push_caret = caret;
	caret.clear();
	screenshoot push;
	proc();
	screenshoot another;
	push.blend(another, milliseconds);
	caret = push_caret;
}

void object_drag_drop() {
	static point drag_offset;
	if(dragactive(last_object))
		caret = hmouse + drag_offset;
	else if(ishilite(32)) {
		if(hkey == MouseLeft && hpressed) {
			drag_offset = caret - hmouse;
			dragbegin(last_object);
		}
	}
}

static void object_paint() {
	auto push_alpha = draw::alpha;
	draw::alpha = alpha;
	last_object->painting();
	draw::alpha = push_alpha;
}

static size_t getobjects(object** pb, object** pe) {
	auto ps = pb;
	for(auto& e : bsdata<object>()) {
		if(!e)
			continue;
		if(ps < pe)
			*ps++ = &e;
	}
	return ps - pb;
}

static int compare(const void* v1, const void* v2) {
	auto p1 = *((object**)v1);
	auto p2 = *((object**)v2);
	auto a1 = p1->priority / 10;
	auto a2 = p2->priority / 10;
	if(a1 != a2)
		return a1 - a2;
	if(p1->position.y != p2->position.y)
		return p1->position.y - p2->position.y;
	if(p1->priority != p2->priority)
		return p1->priority - p2->priority;
	return p1->position.x - p2->position.x;
}

static void sortobjects(object** pb, size_t count) {
	qsort(pb, count, sizeof(pb[0]), compare);
}

void paint_objects() {
	static object* source[max_object_count];
	auto push_caret = caret;
	auto push_clip = clipping;
	objects_mouse = camera + hmouse;
	objects_screen = {caret.x, caret.y, caret.x + width, caret.y + height};
	setclip(objects_screen);
	auto count = getobjects(source, source + sizeof(source) / sizeof(source[0]));
	sortobjects(source, count);
	auto push_object = last_object;
	for(size_t i = 0; i < count; i++) {
		draw::caret = source[i]->getscreen();
		last_object = source[i];
		if(object_before_paint)
			object_before_paint();
		object_paint();
	}
	last_object = push_object;
	clipping = push_clip;
	caret = push_caret;
}

void* choose_object() {
	draw::scene(paint_objects);
	return (void*)getresult();
}

object*	addobject(point screen, const void* data, fnevent painting, unsigned char param, unsigned char priority, unsigned char alpha, unsigned char flags) {
	auto p = bsdata<object>::add();
	p->position = screen;
	p->painting = painting;
	p->data = data;
	p->param = param;
	p->priority = priority;
	p->alpha = alpha;
	p->flags = flags;
	return p;
}

object* findobject(const void* p) {
	for(auto& e : bsdata<object>()) {
		if(e.data == p)
			return &e;
	}
	return 0;
}

void clear_objects() {
	bsdata<object>::source.clear();
}

static rect getcorrectarea(int offs) {
	return {camera.x + offs, camera.y + offs,
		camera.x + objects_screen.width() - offs,
		camera.y + objects_screen.height() - offs};
}

static void correct_camera(point result, int offs) {
	if(objects_screen) {
		rect area = getcorrectarea(offs);
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

static void moving(point& result, point goal, int step, int corrent, fnevent pscene) {
	auto start = result;
	auto maxds = distance(start, goal);
	auto curds = 0;
	while(curds < maxds && ismodal()) {
		result.x = (short)(start.x + (goal.x - start.x) * curds / maxds);
		result.y = (short)(start.y + (goal.y - start.y) * curds / maxds);
		if(corrent)
			correct_camera(result, corrent);
		pscene();
		doredraw();
		waitcputime(1);
		curds += step;
		if(curds > maxds)
			curds = maxds;
	}
	result = goal;
}

void drawable::move(point goal, int speed, int correct, fnevent pscene) {
	if(!pscene)
		pscene = paintstart;
	moving(position, goal, speed, correct, pscene);
}

static point getcameraorigin(point v) {
	auto w = objects_screen.width();
	if(!w)
		w = getwidth();
	auto h = objects_screen.height();
	if(!h)
		h = getheight();
	v.x -= w / 2;
	v.y -= h / 2;
	return v;
}

void setcamera(point v) {
	camera = getcameraorigin(v);
}

void input_camera() {
	const int step = 32;
	if(!hmouse.in(objects_screen))
		return;
	switch(hkey) {
	case KeyLeft: execute(cbsetsht, camera.x - step, 0, &camera.x); break;
	case KeyRight: execute(cbsetsht, camera.x + step, 0, &camera.x); break;
	case KeyUp: execute(cbsetsht, camera.y - step, 0, &camera.y); break;
	case KeyDown: execute(cbsetsht, camera.y + step, 0, &camera.y); break;
	case MouseWheelUp: execute(cbsetsht, camera.y - step, 0, &camera.y); break;
	case MouseWheelDown: execute(cbsetsht, camera.y + step, 0, &camera.y); break;
	case MouseLeft:
		//case MouseRight:
		if(hpressed && !hilite) {
			dragbegin(&camera);
			camera_drag = camera;
		}
		break;
	default:
		if(dragactive(&camera)) {
			hcursor = cursor::All;
			if(hmouse.x >= 0 && hmouse.y >= 0)
				camera = camera_drag + (dragmouse - hmouse);
		}
		break;
	}
}

bool mouseinobjects() {
	return hmouse.in(objects_screen);
}

bool cameravisible(point goal, int border) {
	rect rc = {camera.x, camera.y, camera.x + objects_screen.width(), camera.y + objects_screen.height()};
	rc.offset(-border);
	return goal.in(rc);
}

void slide_camera(point goal, int step) {
	goal = getcameraorigin(goal);
	auto start = camera;
	auto maxds = distance(start, goal);
	if(!maxds)
		return;
	auto curds = 0;
	remove_dead_objects();
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

void focusing(point goal) {
	if(!cameravisible(goal))
		slide_camera(goal);
}

void wait_all() {
	start_timer();
	while(bsdata<draworder>::source.count > 0 && ismodal()) {
		update_timestamp();
		update_all_orders();
		paintstart();
		doredraw();
		waitcputime(1);
		remove_orders();
	}
	remove_dead_objects();
}

void instant_all() {
	for(auto& e : bsdata<draworder>()) {
		if(!e)
			continue;
		e.finish();
	}
	remove_orders();
	remove_dead_objects();
}

void draworder::wait() {
	if(!(*this))
		return;
	start_timer();
	while((*this) && bsdata<draworder>::source.count > 0 && ismodal()) {
		update_timestamp();
		update_all_orders();
		paintstart();
		doredraw();
		waitcputime(1);
		remove_orders();
	}
	remove_dead_objects();
}