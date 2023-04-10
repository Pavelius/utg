#include "bsreq.h"
#include "crt.h"
#include "draw.h"
#include "drawable.h"
#include "screenshoot.h"
#include "variant.h"

using namespace draw;

drawable::fnpaint drawable::painting;

static adat<drawable*, 512> objects;

static unsigned long timestamp, timestamp_last;
static rect last_screen;

long distance(point from, point to);

namespace {
struct orderi : drawable {
	drawable*	parent;
	drawable		start;
	unsigned long tick_start, tick_stop;
	void		update();
};
}

BSDATAC(orderi, 512)

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

inline void copy(drawable& e1, drawable& e2) {
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

static orderi* add_order(drawable* parent, int milliseconds) {
	auto p = bsdata<orderi>::addz();
	copy(*p, *parent);
	copy(p->start, *parent);
	p->tick_start = timestamp;
	p->parent = parent;
	p->tick_stop = p->tick_start + milliseconds;
	return p;
}

static void add_objects(const rect& rc, array& source, unsigned offset) {
	auto pe = (drawable*)(source.end() + offset);
	auto size = source.size;
	for(auto pb = (drawable*)(source.begin() + offset); pb < pe; pb = (drawable*)((char*)pb + size)) {
		if(!pb->priority)
			continue;
		if(!pb->position.in(rc))
			continue;
		objects.add(pb);
	}
}

static void prepare_objects() {
	auto rc = drawable::getscreen(-128);
	for(auto& em : bsdata<varianti>()) {
		if(!em.source || !em.metadata)
			continue;
		auto pm = em.metadata->find("position");
		if(!pm)
			continue;
		if(em.metadata->find("alpha") == 0 || em.metadata->find("priority") == 0)
			continue;
		add_objects(rc, *em.source, pm->offset);
	}
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

bool drawable::iswaitable() const {
	for(auto& e : bsdata<orderi>()) {
		if(e.parent == this)
			return true;
	}
	return false;
}

void drawable::paint() const {
	auto push_alpha = draw::alpha;
	draw::alpha = alpha;
	if(painting)
		painting(this);
	draw::alpha = push_alpha;
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

void drawable::waitall() {
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

void drawable::wait() const {
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

void drawable::splash(unsigned milliseconds) {
	screenshoot push;
	paintstart();
	screenshoot another;
	push.blend(another, milliseconds);
}