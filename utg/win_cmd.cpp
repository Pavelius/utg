#include "slice.h"
#include "stringbuilder.h"
#include "win.h"

char** szcmdargv(int& argc) {
	static char* argv[128];
	memset(argv, 0, sizeof(argv));
	argc = 0;
	const char* p = GetCommandLineA();
	while(*p) {
		if((unsigned)argc >= sizeof(argv) / sizeof(argv[0]))
			break;
		p = skipcr(p);
		const char* pe = 0;
		if(*p == '\"' || *p == '\'') {
			char t = *p++;
			pe = zchr(p, t);
		} else
			pe = zchr(p, ' ');
		if(!pe)
			pe = zend(p);
		char* psz = new char[pe - p + 1];
		memcpy(psz, p, pe - p);
		psz[pe - p] = 0;
		argv[argc++] = psz;
		if(pe[0] == 0)
			break;
		p = skipspcr(pe + 1);
	}
	return argv;
}

const char* get_command_line() {
	return GetCommandLineA();
}