#pragma once

void speech_initialize();
void speech_read(const char* url);

const char* speech_getid(int index);
const char* speech_get(const char* id);