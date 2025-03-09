#include "bsdata.h"
#include "editor.h"
#include "filehandle.h"

BSDATA(filehandle) = {
	{"FilePallette", "*.pal", read_pallette},
	{"FileImages", "*.pma", read_frames}
};
BSDATAF(filehandle)