#include "bsdata.h"
#include "filehandle.h"
#include "io_stream.h"
#include "slice.h"
#include "stringbuilder.h"

bool file_open_dialog(const char* title, char* result, const char* filter, int filter_index, const char* ext);

char current_file[260];

static size_t add_filter(char* result, const filehandle* p) {
	char temp[260]; stringbuilder sb(temp);
	sb.add("%1 (%2)", getnm(p->id), p->filter);
	auto n1 = zlen(temp) + 1;
	if(result) {
		memcpy(result, temp, n1);
		result += n1;
	}
	auto n2 = zlen(p->filter) + 1;
	if(result) {
		memcpy(result, p->filter, n2);
		result += n2;
	}
	return n1 + n2;
}

static size_t get_filter_string(char* result) {
	size_t n = 0;
	for(auto& e : bsdata<filehandle>())
		n += add_filter(result, &e);
	return n;
}

static const char* get_filter_string() {
	auto n = get_filter_string(0);
	auto p = new char[n + 1];
	get_filter_string(p);
	p[n] = 0;
	return p;
}

static filehandle* find_handle(const char* url) {
	for(auto& e : bsdata<filehandle>()) {
		if(szpmatch(url, e.filter))
			return &e;
	}
	return 0;
}

void read_file(const char* url) {
	auto p = find_handle(url);
	if(!p || !p->open)
		return;
	p->open(url);
}

void open_file() {
	auto filter = get_filter_string();
	if(!file_open_dialog(getnm("OpenFile"), current_file, filter, 0, 0)) {
		delete[] filter;
		return;
	}
	read_file(current_file);
}