#pragma once

enum ability_s : unsigned char;
enum tag_s : unsigned char;

int	party_average(ability_s v);
int	party_bolster();
int	party_discount();

void use_bolster();
