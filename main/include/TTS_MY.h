#ifndef TTS_MY_H
#define TTS_MY_H

#include <ESP_I2S.h>

void tts_init();
void tts_speak(const char *str, I2SClass &i2s_out);

#endif // TTS_MY_H