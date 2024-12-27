#include <ESP_I2S.h>
#define INMP441_WS 5
#define INMP441_BCLK 4
#define INMP441_DIN 6
#define MAX98357_LRC 16
#define MAX98357_BCLK 15
#define MAX98357_DIN 7

I2SClass i2s, i2s_out;

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