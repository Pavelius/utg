#include "answers.h"
#include "player.h"

#pragma once

extern const char* last_id;

struct pushtitle {
	const char *header, *resid, *push_id;
	pushtitle(const char* id) : push_id(last_id), header(answers::header), resid(answers::resid) {
		last_id = id;
		answers::header = getnm(id);
		answers::resid = player->id;
	}
	~pushtitle() {
		last_id = push_id;
		answers::header = header;
		answers::resid = resid;
	}
};