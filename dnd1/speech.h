#pragma once

void speech_initialize();
void speech_read(const char* url);

int speech_count(const char* id);
int speech_first(const char* id);
int speech_random(const char* id);

const char* speech_name(int index);
const char* speech_getid(int index);
const char* speech_get(const char* id);