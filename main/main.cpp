// #include "my_spiffs.h"
#include <Arduino.h>
#include "ESP_I2S.h"
#include "ESP_SR.h"

#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"
#define TAG ""
#else
#include "esp_log.h"
static const char *TAG = "main";
#endif

#define INMP441_WS 5
#define INMP441_BCLK 4
#define INMP441_DIN 6

I2SClass i2s;

enum
{
    SR_CMD_HELLO,
};
static const sr_cmd_t sr_commands[] = {
    {0, "Turn on the light", "TkN nN jc LiT"},
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

void setup(void)
{
    ESP_LOGE(TAG, "Hello, world!");
    Serial.begin(115200);
    // Serial.printf("ijWDSNNQASDJLAKSJDSAKJD");
    // bsp_spiffs_mount();
    i2s.setPins(INMP441_BCLK, INMP441_WS, -1, INMP441_DIN);
    i2s.setTimeout(1000);
    i2s.begin(I2S_MODE_STD, 16000, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);

    ESP_SR.onEvent(onSrEvent);
    ESP_SR.begin(i2s, sr_commands, sizeof(sr_commands) / sizeof(sr_cmd_t), SR_CHANNELS_STEREO, SR_MODE_WAKEWORD);
}

void loop()
{
    // Serial.println("Hello, world!");
    // printf("Hello, world!\n");
    // delay(1000);
}