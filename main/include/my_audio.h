#include <esp_err.h>
#ifndef MY_AUDIO_H
#define MY_AUDIO_H

#define CODEC_DEFAULT_SAMPLE_RATE (16000)
#define CODEC_DEFAULT_BIT_WIDTH (16)
#define CODEC_DEFAULT_ADC_VOLUME (24.0)
#define CODEC_DEFAULT_CHANNEL (2)

static esp_err_t bsp_codec_init();

#endif // MY_AUDIO_H