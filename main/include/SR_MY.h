#ifndef SR_MY_H
#define SR_MY_H
#include <ESP_I2S.h>

void ESP_SR_addCommand(int id, const char *str, const char *phoneme);

void i2s_init();
void ESP_SR_init();

#endif // SR_MY_H