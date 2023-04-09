#include "crt.h"
#include "draw.h"
#include "draw_posable.h"
#include "screenshoot.h"

using namespace draw;

posable::fnpaint posable::painting;
posable::fnget posable::getposables;

static unsigned long timestamp, timestamp_last;
static rect last_screen;

long distance(point from, point to);

namespace {
struct orderi : posable {
	posable*	parent;
	posable		start;
	unsigned long tick_start, tick_stop;
	void		update();
};
}

BSDATAC(orderi, 512)

static void correct_camera(point result, int offs) {
	if(last_screen) {
		rect area = posable::getscreen(offs);
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
	while(curds < maxds && ismodal()) {
		result.x = (short)(start.x + (goal.x - start.x) * curds / maxds);
		result.y = (short)(start.y + (goal.y - start.y) * curds / maxds);
		if(corrent)
			correct_camera(result, corrent);
		paintstart();
		doredraw();
		waitcputime(1);
		curds += step;
		if(curds > maxds)
			curds = maxds;
	}
	result = goal;
}

inline void copy(posable& e1, posable& e2) {
	e1 = e2;
}

static void remove_orders() {
	auto pb = bsdata<orderi>::begin();
	auto pe = bsdata<orderi>::end();
	while(pe > pb) {
		pe--;
		if(*pe)
			break;
		bsdata<orderi>::source.count--;
	}
}

static void update_all_orders() {
	for(auto& e : bsdata<orderi>()) {
		if(e)
			e.update();
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

static int calculate(int v1, int v2, int n, int m) {
	return v1 + (v2 - v1) * n / m;
}

void orderi::update() {
	if(tick_start > timestamp)
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
}

static orderi* add_order(posable* parent, int milliseconds) {
	auto p = bsdata<orderi>::addz();
	copy(*p, *parent);
	copy(p->start, *parent);
	p->tick_start = timestamp;
	p->parent = parent;
	p->tick_stop = p->tick_start + milliseconds;
	return p;
}

static size_t getobjects(posable** pb, posable** pe) {
}

static int compare(const void* v1, const void* v2) {
	auto p1 = *((posable**)v1);
	auto p2 = *((posable**)v2);
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

static void sortobjects(posable** pb, size_t count) {
	qsort(pb, count, sizeof(pb[0]), compare);
}

void posable::clear() {
	memset(this, 0, sizeof(*this));
}

bool posable::iswaitable() const {
	for(auto& e : bsdata<orderi>()) {
		if(e.parent == this)
			return true;
	}
	return false;
}

void posable::paint() const {
	auto push_alpha = draw::alpha;
	draw::alpha = alpha;
	if(painting)
		painting(this);
	draw::alpha = push_alpha;
}

void posable::paintall() {
	static posable::collection_type source;
	if(!getposables)
		return;
	auto push_caret = caret;
	auto push_clip = clipping;
	last_screen = {caret.x, caret.y, caret.x + width, caret.y + height};
	setclip(last_screen);
	source.clear();
	getposables(source);
	sortobjects(source.data, source.count);
	for(auto p : source) {
		draw::caret = p->position - camera;
		p->paint();
	}
	clipping = push_clip;
	caret = push_caret;
}

rect posable::getscreen(int offs) {
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

void posable::focusing() const {
	const int offset = 16;
	if(!position.in(posable::getscreen(offset)))
		posable::slide(position, offset);
}

void posable::slide(point goal, int step) {
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

void posable::waitall() {
	start_timer();
	while(bsdata<orderi>::source.count > 0 && ismodal()) {
		update_timestamp();
		update_all_orders();
		paintstart();
		doredraw();
		waitcputime(1);
		remove_orders();
	}
}

void posable::wait() const {
	if(!iswaitable())
		return;
	start_timer();
	while(bsdata<orderi>::source.count > 0 && iswaitable() && ismodal()) {
		update_timestamp();
		update_all_orders();
		paintstart();
		doredraw();
		waitcputime(1);
		remove_orders();
	}
}

void posable::splash(unsigned milliseconds) {
	screenshoot push;
	paintstart();
	screenshoot another;
	push.blend(another, milliseconds);
}