#include "crt.h"
#include "io_stream.h"
#include "log.h"
#include "stringbuilder.h"

namespace {
struct translate {
	const char*			id;
	const char*			name;
};
}

static char main_locale[4];
static array source_name(sizeof(translate));
static array source_text(sizeof(translate));

static int compare(const void* v1, const void* v2) {
	auto p1 = (const translate*)v1;
	auto p2 = (const translate*)v2;
	return strcmp(p1->id, p2->id);
}

static void update_elements(array& ei) {
	if(ei.getcount() == 0)
		return;
	qsort(ei.data, ei.getcount(), ei.getsize(), compare);
}

static const char* read_string_v1(const char* p, char* ps, const char* pe) {
	char sym;
	while(*p && *p != '\n' && *p != '\r') {
		if(p[0] == '\\' && p[1] == 'n') {
			sym = '\n';
			p += 2;
		} else {
			sym = *p;
			p++;
		}
		switch(sym) {
		case -72: sym = '�'; break;
		case -105: case 17: sym = '-'; break;
		}
		if(ps < pe)
			*ps++ = sym;
	}
	*ps = 0;
	while(*p == '\n' || *p == '\r') {
		p = skipcr(p);
		p = skipsp(p);
	}
	return p;
}

static const char* read_identifier(const char* p, char* ps, const char* pe) {
	while(*p && (ischa(*p) || isnum(*p) || *p == '_' || *p == ' ')) {
		if(ps < pe)
			*ps++ = *p++;
		else
			break;
	}
	*ps = 0;
	return p;
}

static void apply_value(array& source, const char* id, const char* name) {
	id = szdup(id);
	name = szdup(name);
	if(source.find(id, 0) != -1)
		return;
	auto p = (translate*)source.add();
	p->id = id;
	p->name = name;
}

static void readl(const char* url, array& source, bool required) {
	auto p = log::read(url, required);
	if(!p)
		return;
	char name[128], value[8192];
	auto records_read = 0;
	while(*p) {
		p = read_identifier(p, name, name + sizeof(name) - 1);
		if(p[0] != ':')
			break;
		p = skipsp(p + 1);
		p = read_string_v1(p, value, value + sizeof(value) - 1);
		apply_value(source, name, value);
		records_read++;
	}
	log::close();
	update_elements(source);
}

static void savel(const char* url, array& source, bool only_empthy) {
	io::file file(url, StreamText | StreamWrite);
	if(!file)
		return;
	auto records_write = 0;
	for(auto& e : source.records<translate>()) {
		if(only_empthy && e.name)
			continue;
		file << e.id << ": ";
		if(e.name) {
			file << e.name;
			records_write++;
		}
		file << "\r\n";
	}
}

static void setfile(array& source, const char* folder, const char* id, const char* locale, bool write_mode, bool required, bool only_empthy) {
	char temp[260]; stringbuilder sb(temp);
	sb.clear(); sb.addlocalefile(folder, id, 0);
	if(write_mode)
		savel(temp, source, only_empthy);
	else
		readl(temp, source, required);
}

static void setlist(array& source, const char* folder, const char* locale) {
	char temp[260]; stringbuilder sb(temp);
	sb.clear(); sb.addlocaleurl(); sb.add("/%1/", folder);
	char filter[260]; stringbuilder sf(filter);
	sf.add("*.txt");
	for(io::file::find find(temp); find; find.next()) {
		auto pn = find.name();
		if(pn[0] == '.')
			continue;
		if(!szpmatch(pn, filter))
			continue;
		char file[512];
		readl(find.fullname(file), source, false);
	}
}

static void deinitialize() {
	setfile(source_name, "names", "NamesNewbe", main_locale, true, false, true);
}

static void check(array& source, const char* locale, const char* url) {
	log::seturl(url);
	for(auto& e : source.records<translate>()) {
		if(e.name && e.name[0])
			continue;
		log::error(0, "%1: Define translate", e.id);
	}
}

void check_translation() {
	check(source_name, main_locale, "Names.txt");
}

static void copy_locale(const char* locale) {
	stringbuilder sb(main_locale);
	sb.add(locale);
}

const char* getcurrentlocale() {
	return main_locale;
}

void initialize_translation(const char* locale) {
	if(main_locale[0])
		return;
	copy_locale(locale);
	setfile(source_name, "names", "Names", main_locale, false, true, false);
	setlist(source_name, "names", main_locale);
	setfile(source_text, "tips", "Descriptions", main_locale, false, false, false);
	setlist(source_text, "tips", main_locale);
	atexit(deinitialize);
}

const char* getnm(const char* id) {
	if(!id || id[0] == 0)
		return "";
	if(isnum(id[0]))
		return id;
	translate key = {id, 0};
	auto p = (translate*)bsearch(&key, source_name.data, source_name.getcount(), source_name.getsize(), compare);
	if(!p) {
#ifdef _DEBUG
		p = (translate*)source_name.add();
		memset(p, 0, sizeof(*p));
		p->id = szdup(id);
		update_elements(source_name);
#endif
		return id;
	}
	if(!p->name || !p->name[0])
		return id;
	return p->name;
}

const char* getnme(const char* id) {
	if(!id || id[0] == 0)
		return "";
	translate key = {id, 0};
	auto p = (translate*)bsearch(&key, source_name.data, source_name.getcount(), source_name.getsize(), compare);
	if(!p || !p->name || !p->name[0])
		return 0;
	return p->name;
}

const char* getdescription(const char* id) {
	if(!id || id[0] == 0)
		return 0;
	translate key = {id, 0};
	auto p = (translate*)bsearch(&key, source_text.data, source_text.getcount(), source_text.getsize(), compare);
	if(!p || !p->name)
		return 0;
	return p->name;
}