#include "bsdata.h"
#include "editor.h"
#include "filehandle.h"

BSDATA(filehandle) = {
	{"FilePallette", "*.pal", read_pallette}
};
BSDATAF(filehandle)