// #include "my_spiffs.h"
#include <Arduino.h>
#include "item_manager.h"
#include "I2S_MY.h"
#include "TTS_MY.h"
#include "SR_MY.h"
#include "ESP_I2S.h"
#include "SPIFFS.h"
#include "HX711.h"

#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"
#define TAG ""
#else
#include "esp_log.h"
static const char *TAG = "main";
#endif
#include "http_manager.h"

#define HX711_VCC 20
#define HX711_SCK 21
#define HX711_DOUT 47

Hx711 hx711(HX711_DOUT, HX711_SCK);

void HX711_init()
{
    pinMode(HX711_VCC, OUTPUT);
    digitalWrite(HX711_VCC, HIGH);
    hx711.setScale(400.0);
    hx711.setOffset(0);
    Serial.println("HX711 Initialized!");
}

const int WT_THR = 2;

void setup(void)
{
    Wifi_init();
    ESP_LOGE(TAG, "Hello, world!");
    Serial.begin(115200);
    i2s_init();
    tts_init();
    ESP_SR_init();
    HX711_init();
    itemManager.setWtThreshold(WT_THR);
    tts_speak("欢迎使用多可智能抽屉", i2s_out);
}

int nowWeight = 0;
int lastStableWeight = -1;
int stableCnt = 0;
const int STB_THR = 3;
void updateWeights()
{
    int wt = hx711.getWeight(10);
    // log_i("Weight: %d\n", wt);
    if (abs(wt - nowWeight) > 1)
    {
        stableCnt = 0;
    }
    else if (stableCnt > STB_THR)
    {
        if (lastStableWeight != -1 && abs(wt - lastStableWeight) > WT_THR)
        {
            int diff = wt - lastStableWeight;
            int id = itemManager.searchItem(abs(diff));
            if (id != -1)
            {
                itemManager.addItemCnt(id, diff / abs(diff));
                const char *name = itemManager.getItemName(id);
                log_i("Item %d:%s, weight: %d detected!\n", id, name, diff);
                if (diff > 0)
                {
                    tts_speak("放入了", i2s_out);
                }
                else
                {
                    tts_speak("拿出了", i2s_out);
                }
                if (strlen(name) > 0)
                {
                    tts_speak(name, i2s_out);
                }
                else
                {
                    tts_speak("未命名物品", i2s_out);
                }
            }
            else
            {
                log_i("Unknown Item weight: %d detected!\n", diff);
                // log_i("Item not found!\n");
                if (diff > 0)
                {
                    itemManager.addItem("", "", diff, 1);
                    tts_speak("放入了未知物品", i2s_out);
                }
                else
                {
                    log_e("Item not found taken out!\n");
                }
            }
        }
        lastStableWeight = wt;
    }
    else
    {
        stableCnt++;
    }
    nowWeight = wt;
}
int lastDownloadMillis = 0;
const int WT_UPDATE_INTERVAL = 10000;
void loop()
{
    updateWeights();
    if (millis() - lastDownloadMillis > WT_UPDATE_INTERVAL)
    {
        itemManager.downloadItems();
        lastDownloadMillis = millis();
    }
    delay(1);
}