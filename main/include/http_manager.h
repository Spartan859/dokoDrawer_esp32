#ifndef HTTP_MANAGER_H
#define HTTP_MANAGER_H

#include <ArduinoJson.h>
void Wifi_init();
void postJson(DynamicJsonDocument &jdoc);
void getJson(DynamicJsonDocument &jdoc);

#endif // HTTP_MANAGER_H