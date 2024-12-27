#include "TTS_MY.h"
#include <esp_tts.h>
#include "esp_tts_voice.h"
#include "esp_tts_voice_xiaole.h"
#include <spi_flash_mmap.h>
#include <esp_tts.h>
#include <esp_tts_voice_template.h>
#include <esp_partition.h>
#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"
#define TAG ""
#else
#include "esp_log.h"
static const char *TAG = "TTS_MY";
#endif
#include <ESP_I2S.h>

esp_tts_handle_t tts_handle;
int tts_enabled = 0;
void tts_init()
{
    const esp_partition_t *part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, "voice_data");
    if (part == 0)
    {
        throw "voice_data partition not found";
    }
    else
    {
        log_i("voice_data paration size:%d\n", part->size);
    }
    const void *voicedata;
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
    esp_partition_mmap_handle_t mmap;
    esp_err_t err = esp_partition_mmap(part, 0, part->size, ESP_PARTITION_MMAP_DATA, &voicedata, &mmap);
#else
    spi_flash_mmap_handle_t mmap;
    esp_err_t err = esp_partition_mmap(part, 0, part->size, SPI_FLASH_MMAP_DATA, &voicedata, &mmap);
#endif
    if (err != ESP_OK)
    {
        throw "Couldn't map voice data partition!\n";
    }
    esp_tts_voice_t *voice = esp_tts_voice_set_init(&esp_tts_voice_template, (int16_t *)voicedata);
    tts_handle = esp_tts_create(voice);
    tts_enabled = 1;

    // char *str = "欢迎使用乐鑫语音合成";
    // log_i("str:%s\n", str);
    // if (esp_tts_parse_chinese(tts_handle, str))
    // {
    //     int len[1] = {0};
    //     do
    //     {
    //         short *pcm_data = esp_tts_stream_play(tts_handle, len, 3);
    //         // convert pcm to wav_buffer
    //         i2s_out.write((uint8_t *)pcm_data, len[0] * 2);
    //         // printf("data:%d \n", len[0]);
    //     } while (len[0] > 0);
    // }
    // esp_tts_stream_reset(tts_handle);
}

void tts_speak(const char *str, I2SClass &i2s_out)
{
    if (!tts_enabled)
    {
        return;
    }
    if (esp_tts_parse_chinese(tts_handle, str))
    {
        int len[1] = {0};
        do
        {
            short *pcm_data = esp_tts_stream_play(tts_handle, len, 3);
            // convert pcm to wav_buffer
            i2s_out.write((uint8_t *)pcm_data, len[0] * 2);
            // printf("data:%d \n", len[0]);
        } while (len[0] > 0);
    }
    esp_tts_stream_reset(tts_handle);
}