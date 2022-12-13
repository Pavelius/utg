#pragma once

enum tag_s : unsigned char {
	ArrowProof, Blunted, CatfolkSteel, Comfortable, Cumbersome, Durable, Eaglecraft, Flexible, Large, Makeshift, MetalBoss,
	Precise, Quick, Reach, Reinforced, Sharp, Shoddy, SteelOfRabbitfolk, SteelOfFoxfolk,
	TightlyWoven, Throwable, Unassuming, Weighty,
	Positive, Negative, Load, MarkWear, MarkExhaustion,
	Armor, Intimate, Close, Far,
};
struct tagi {
	const char*		id;
	unsigned		flags;
	bool			is(tag_s v) const { return (flags & (1 << v)) != 0; }
};
