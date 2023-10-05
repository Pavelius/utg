#include "answers.h"
#include "player.h"

#pragma once

struct pushtitle {
	const char *header, *resid;
	pushtitle(const char* id) : header(answers::header), resid(answers::resid) {
		answers::header = getnm(id);
		answers::resid = player->id;
	}
	~pushtitle() {
		answers::header = header;
		answers::resid = resid;
	}
};