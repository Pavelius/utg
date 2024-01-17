#pragma once

struct speech;

void speech_initialize();
void speech_read(const char* url);

const speech* speech_find(const char* id);

int speech_count(const speech* p);
int speech_first(const speech* p);
int speech_random(const char* id);

const char* speech_name(int index);
const char* speech_getid(int index);
const char* speech_get(const char* id);
const char* speech_get(const speech* p, int n);

extern unsigned char* speech_params;