// #include "my_spiffs.h"
#include <Arduino.h>
#include "ESP_I2S.h"
#include "ESP_SR.h"
#include "SPIFFS.h"
#include "HX711.h"
#include "esp_tts_voice.h"
#include "esp_tts_voice_xiaole.h"

#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"
#define TAG ""
#else
#include "esp_log.h"
static const char *TAG = "main";
#endif
#include <spi_flash_mmap.h>
#include <esp_tts.h>
#include <esp_tts_voice_template.h>

#define INMP441_WS 5
#define INMP441_BCLK 4
#define INMP441_DIN 6

#define MAX98357_LRC 16
#define MAX98357_BCLK 15
#define MAX98357_DIN 7

#define HX711_VCC 20
#define HX711_SCK 21
#define HX711_DOUT 47

I2SClass i2s, i2s_out;

Hx711 hx711(HX711_DOUT, HX711_SCK);

enum
{
    SR_CMD_HELLO,
};
static const sr_cmd_t sr_commands[] = {
    // {0, "Turn on the light", "TkN nN jc LiT"},
    {0, "你好", "ni hao"},
};

void onSrEvent(sr_event_t event, int command_id, int phrase_id)
{
    switch (event)
    {
    case SR_EVENT_WAKEWORD:
        Serial.println("WakeWord Detected!");
        break;
    case SR_EVENT_WAKEWORD_CHANNEL:
        Serial.printf("WakeWord Channel %d Verified!\n", command_id);
        ESP_SR.setMode(SR_MODE_COMMAND); // Switch to Command detection
        break;
    case SR_EVENT_TIMEOUT:
        Serial.println("Timeout Detected!");
        ESP_SR.setMode(SR_MODE_WAKEWORD); // Switch back to WakeWord detection
        break;
    case SR_EVENT_COMMAND:
        Serial.printf("Command %d Detected! %s\n", command_id, sr_commands[phrase_id].str);
        switch (command_id)
        {
        case SR_CMD_HELLO:
            Serial.println("Hello, world!");
            break;
        default:
            Serial.println("Unknown Command!");
            break;
        }
        ESP_SR.setMode(SR_MODE_COMMAND); // Allow for more commands to be given, before timeout
        // ESP_SR.setMode(SR_MODE_WAKEWORD); // Switch back to WakeWord detection
        break;
    default:
        Serial.println("Unknown Event!");
        break;
    }
}

void i2s_init()
{
    i2s.setPins(INMP441_BCLK, INMP441_WS, -1, INMP441_DIN);
    i2s_out.setPins(MAX98357_BCLK, MAX98357_LRC, MAX98357_DIN, -1);
    i2s.setTimeout(1000);
    i2s_out.setTimeout(1000);
    if (!i2s.begin(I2S_MODE_STD, 16000, I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO, I2S_STD_SLOT_LEFT))
    {
        Serial.println("Failed to initialize I2S bus!");
        return;
    }
    if (!i2s_out.begin(I2S_MODE_STD, 16000, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO, I2S_STD_SLOT_RIGHT))
    {
        Serial.println("Failed to initialize I2S bus out!");
        return;
    }
    if (!i2s.configureRX(16000, I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO, I2S_RX_TRANSFORM_32_TO_16))
    {
        Serial.println("Failed to configure I2S RX transformation!");
        return;
    }
    // if (!i2s_out.configureTX(16000, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO, I2S_STD_SLOT_RIGHT))
    // {
    //     Serial.println("Failed to configure I2S TX!");
    //     return;
    // }
}

void ESP_SR_init()
{
    ESP_SR.onEvent(onSrEvent);
    ESP_SR.begin(i2s, sr_commands, sizeof(sr_commands) / sizeof(sr_cmd_t), SR_CHANNELS_MONO, SR_MODE_WAKEWORD);
}

void HX711_init()
{
    pinMode(HX711_VCC, OUTPUT);
    digitalWrite(HX711_VCC, HIGH);
    hx711.setScale(400.0);
    hx711.setOffset(0);
    Serial.println("HX711 Initialized!");
}

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
    esp_tts_handle_t tts_handle = esp_tts_create(voice);

    char *str = "欢迎使用乐鑫语音合成";
    log_i("str:%s\n", str);
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

void setup(void)
{
    ESP_LOGE(TAG, "Hello, world!");
    Serial.begin(115200);
    i2s_init();
    tts_init();
    // ESP_SR_init();
    uint8_t *wav_data;
    size_t wav_size;
    // Serial.println("Recording 10 seconds of audio data...");
    // wav_data = i2s.recordWAV(10, &wav_size);
    // Serial.println("Recording complete. Playing audio data in 3 seconds.");
    // delay(3000);
    // Serial.println("Playing audio data...");
    // i2s_out.playWAV(wav_data, wav_size);

    // if (!SPIFFS.begin(false))
    // {
    //     Serial.println("An Error has occurred while mounting SPIFFS");
    //     return;
    // }
    // File wav_file = SPIFFS.open("/Cellphone_Alarm_Clock_II.wav", "r");
    // if (!wav_file)
    // {
    //     Serial.println("Failed to open file for reading");
    //     return;
    // }
    // wav_size = wav_file.size();
    // wav_data = (uint8_t *)malloc(wav_size);
    // if (wav_data == NULL)
    // {
    //     Serial.println("Failed to allocate memory for WAV data");
    //     return;
    // }
    // wav_file.read(wav_data, wav_size);
    // wav_file.close();
    // Serial.println("Playing audio data...");
    // i2s_out.playWAV(wav_data, wav_size);
    // free(wav_data);
    // HX711_init();
}

void loop()
{
    // Serial.println("Hello, world!");
    // printf("Hello, world!\n");
    // printf("Weight: %lf\n", hx711.getWeight(10));
    delay(1000);
}