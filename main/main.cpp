#include "Arduino.h"

void setup()
{
    Serial.begin(115200);
}

void loop()
{
    delay(1000);
    Serial.println("Hello World");
    // blink the LED
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
}