#include "my_spiffs.h"
#include <Arduino.h>

#if defined(ARDUINO_ARCH_ESP32) && defined(CONFIG_ARDUHAL_ESP_LOG)
#include "esp32-hal-log.h"
#define TAG ""
#else
#include "esp_log.h"
static const char *TAG = "main";
#endif

void setup(void)
{
    // log_d("DEBUG: DEBUG, WORLD\n");
    // log_e("ERROR: ERROR, WORLD\n");
    // Serial.printf("ijWDSNNQASDJLAKSJDSAKJD");
    bsp_spiffs_mount();
}

void loop()
{
    // Serial.println("Hello, world!");
    printf("Hello, world!\n");
    delay(1000);
}