#include "http_manager.h"
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

WiFiClient client;
HTTPClient http;
String wifi_ssid = "hoshizora";
String wifi_password = "zteztezte";
String host = "http://101.133.137.243:2736";
String upd = "/api/v1/update_items";
String get = "/api/v1/get_items";

void update_arguments_from_spiffs()
{
    if (!SPIFFS.begin())
    {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }
    File file = SPIFFS.open("/web_config.json", "r");
    if (!file)
    {
        Serial.println("Failed to open file for reading");
        return;
    }
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, file);
    if (error)
    {
        Serial.println("Failed to read file, using default configuration");
        return;
    }
    if (!doc["wifi_ssid"].isNull())
    {
        wifi_ssid = doc["wifi_ssid"].as<String>();
    }
    if (!doc["wifi_password"].isNull())
    {
        wifi_password = doc["wifi_password"].as<String>();
    }
    if (!doc["host"].isNull())
    {
        host = doc["host"].as<String>();
    }
    if (!doc["upd"].isNull())
    {
        upd = doc["upd"].as<String>();
    }
    if (!doc["get"].isNull())
    {
        get = doc["get"].as<String>();
    }
}

void Wifi_init()
{
    update_arguments_from_spiffs();
    WiFi.begin(wifi_ssid, wifi_password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the WiFi network");
}

void postJson(DynamicJsonDocument &jdoc)
{
    String json;
    serializeJson(jdoc, json);
    http.begin(client, host + upd);
    http.POST(json);
    http.end();
}

void getJson(DynamicJsonDocument &jdoc)
{
    http.begin(client, host + get);
    http.GET();
    deserializeJson(jdoc, http.getStream());
    http.end();
}