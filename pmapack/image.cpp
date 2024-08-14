#include "bsreq.h"
#include "image.h"
#include "io_stream.h"
#include "logparse.h"
#include "pushvalue.h"

using namespace log;

static const char*	p;
static char			last_string[512];
static int			last_value;
imagei				last_image;

static void skipws() {
	p = skipws(p);
}

static bool match(const char* symbol) {
	auto i = 0;
	while(symbol[i]) {
		if(p[i] != symbol[i])
			return false;
		i++;
	}
	p += i;
	skipws();
	return true;
}

static void skipline() {
	while(*p && *p != 13 && *p != 10) {
		if(*p == '\\') {
			p++;
			if(*p == 13 || *p == 10)
				p = skipspcr(p + 1);
			else
				p++;
		} else
			p++;
	}
}

static void nextline() {
	skipline();
	p = skipspcr(p);
}

static void skip(const char* command) {
	if(!allowparse)
		return;
	if(!match(command)) {
		error(p, "Expected token `%1`", command);
		allowparse = false;
	}
}

static void readid() {
	stringbuilder sb(last_string); sb.clear();
	if(!allowparse)
		return;
	auto p_push = p;
	while(*p && (ischa(*p) || isnum(*p) || *p == '_'))
		sb.add(*p++);
	if(p_push == p) {
		error(p, "Expected identifier");
		allowparse = false;
	}
	skipws();
}

static void read_value() {
	last_value = 0;
	last_string[0] = 0;
	if(match("true"))
		last_value = 1;
	else if(match("false"))
		last_value = 0;
	else if(match("default"))
		last_value = DefaultImageNumber;
	else if(ischa(*p)) {
		readid();
	} else if(*p == '"') {
		stringbuilder sb(last_string);
		p = sb.psstr(p + 1, '"');
		skipws();
	} else if(isnum(*p) || *p == '-') {
		auto minus = false;
		if(*p == '-') {
			minus = true;
			p++;
		}
		p = stringbuilder::read(p, last_value);
		if(minus)
			last_value = -last_value;
		skipws();
	} else {
		error(p, "Unknown value");
		allowparse = false;
	}
}

static void write_value(void* object, const bsreq* req, int index) {
	if(!req)
		return;
	auto p1 = req->ptr(object, index);
	if(req->is(KindNumber) || req->is(KindEnum) || req->type == bsmeta<variant>::meta)
		req->set(p1, last_value);
	else if(req->is(KindText))
		req->set(p1, (long)szdup(last_string));
	else if(req->is(KindScalar)) {
		if(req->count > 1)
			write_value(req->ptr(object, index), req->type, 0);
		else
			write_value(req->ptr(object), req->type + index, 0);
	} else if(req->is(KindFlags)) {
		auto data = (unsigned char*)req->ptr(object);
		data[last_value / 8] |= 1 << (last_value % 8);
	} else
		error(p, "Unknown type in requisit `%1`", req->id);
}

static void read_field(void* object, const bsreq* req) {
	readid();
	if(!allowparse)
		return;
	auto pr = req->find(last_string);
	if(!pr)
		error(p, "Can\'t find requisit `%1`", last_string);
	skip("(");
	auto index = 0;
	while(*p) {
		read_value();
		if(pr)
			write_value(object, pr, index++);
		if(match(")"))
			break;
	}
}

static void read_fields(void* object, const bsreq* req, int key) {
	for(auto i = 0; allowparse && i < key; i++) {
		read_value();
		write_value(object, req + i, 0);
	}
	req = req + key;
	while(allowparse && ischa(*p))
		read_field(object, req);
}

static void create_image(imagei* p) {
	p->center = {DefaultImageNumber, DefaultImageNumber};
	p->size = {DefaultImageNumber, DefaultImageNumber};
	p->position = {DefaultImageNumber, DefaultImageNumber};
	p->param = DefaultImageNumber;
}

static void parse() {
	while(allowparse && *p) {
		skip("#");
		auto header = bsdata<imagea>::add();
		memset(header, 0, sizeof(*header));
		create_image(header);
		read_fields(header, bsmeta<imagea>::meta, 1);
		nextline();
		header->elements.setbegin();
		while(ischa(*p)) {
			auto element = bsdata<imagei>::add();
			memset(element, 0, sizeof(*element));
			create_image(element);
			read_fields(element, bsmeta<imagei>::meta, 1);
			nextline();
		}
		header->elements.setend();
	}
}

static void image_after_read() {
	for(auto& e : bsdata<imagei>()) {
		if(e.object)
			continue;
		e.object = bsdata<imagea>::find(e.id);
		if(!e.object)
			e.object = bsdata<imageplugini>::find(e.id);
		if(!e.object)
			error(0, " Don`t find image plugin of list `%1`", e.id);
	}
}

void image_read(const char* url) {
	pushvalue push(context);
	pushvalue push_parser(p);
	p = read(url);
	if(!p)
		return;
	skipws();
	parse();
	close();
	image_after_read();
}

const char* image_source_url() {
	static char temp[1024]; stringbuilder sb(temp); sb.clear();
	if(last_image.url && last_image.url[0]) {
		sb.add(last_image.url);
		sb.add("/");
	}
	if(last_image.name && last_image.name[0])
		sb.add(last_image.name);
	if(last_image.ext && last_image.ext[0]) {
		sb.add(".");
		sb.add(last_image.ext);
	}
	return temp;
}

const char* image_dest_url() {
	static char temp[1024]; stringbuilder sb(temp); sb.clear();
	if(last_image.dest_url && last_image.dest_url[0]) {
		sb.add(last_image.dest_url);
		sb.add("/");
	}
	if(last_image.name && last_image.name[0]) {
		sb.add(last_image.name);
		sb.add(".pma");
	}
	return temp;
}

static void add_value(point& d, point s) {
	if(s.x != DefaultImageNumber)
		d.x = s.x;
	if(s.y != DefaultImageNumber)
		d.y = s.y;
}

static void add_value(int& d, int s) {
	if(s != DefaultImageNumber)
		d = s;
}

static void add_value(const char*& d, const char* s) {
	if(!s)
		return;
	char temp[512]; stringbuilder sb(temp);
	sb.add(s, d);
	d = szdup(temp);
}

static void add_image(const imagei& e) {
	add_value(last_image.url, e.url);
	add_value(last_image.dest_url, e.dest_url);
	add_value(last_image.name, e.name);
	add_value(last_image.ext, e.ext);
	add_value(last_image.param, e.param);
	add_value(last_image.center, e.center);
	add_value(last_image.position, e.position);
	add_value(last_image.size, e.size);
}

static void image_run(const imagea& list) {
	auto push_image = last_image;
	add_image(list);
	auto core_copy = last_image;
	for(auto& e : list.elements) {
		last_image = core_copy; add_image(e);
		if(bsdata<imagea>::have(e.object)) {
			auto p = (imagea*)e.object;
			image_run(*p);
		} else if(bsdata<imageplugini>::have(e.object)) {
			auto p = (imageplugini*)e.object;
			p->proc();
		}
	}
	last_image = push_image;
}

void image_run(const char* id) {
	auto p = bsdata<imagea>::find(id);
	if(!p)
		return;
	auto push_image = last_image;
	memset(&last_image, 0, sizeof(last_image));
	last_image.center = {-1, -1};
	last_image.size = {-1, -1};
	last_image.bpp = 32;
	image_run(*p);
	last_image = push_image;
}