#pragma once

enum tag_s : unsigned char {
	Used, Infantry, Fast, Fly, AnimateSkeletons,
	Forest, Mountains, Farmlands, Sea
};
struct taga {
	unsigned	tags;
	bool		is(tag_s v) const { return (tags & (1 << v)) != 0; }
	void		set(tag_s v) { tags |= (1 << v); }
	void		remove(tag_s v) { tags &= ~(1 << v); }
};
extern tag_s last_tag;