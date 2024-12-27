#include "item_manager.h"
#include <string.h>
#include <esp32-hal-log.h>
#include <ArduinoJson.h>
#include "http_manager.h"
#include "SR_MY.h"
#define ABS(a) ((a) > 0 ? (a) : -(a))

ItemManager::ItemManager()
{
    numItems = 0;
    wt_threshold = 2;
}

ItemManager::~ItemManager()
{
}

void ItemManager::addItemCnt(int id, int cnt)
{
    items[id].cnt += cnt;
    uploadItems();
}

int ItemManager::searchItem(int wt)
{
    int id = -1;
    for (int i = 0; i < numItems; i++)
    {
        log_w("Item %d:%s, weight: %d\n, diff: %d\n", i, items[i].name, items[i].weight, ABS(items[i].weight - wt));
        if (ABS(items[i].weight - wt) <= wt_threshold && (id == -1 || ABS(items[i].weight - wt) < ABS(items[id].weight - wt)))
        {
            id = i;
        }
    }
    return id;
}

void ItemManager::setItem(int id, char *name, char *phoneme)
{
    strcpy(items[id].name, name);
    strcpy(items[id].phoneme, phoneme);
}

void ItemManager::addItem(const char *name, const char *phoneme, int weight, int cnt)
{
    strcpy(items[numItems].name, name);
    strcpy(items[numItems].phoneme, phoneme);
    items[numItems].weight = weight;
    items[numItems].cnt = cnt;
    log_i("Item %d:%s, weight: %d added!\n", numItems, name, weight);
    numItems++;
    uploadItems();
}

void ItemManager::setWtThreshold(int wtThr)
{
    wt_threshold = wtThr;
}

int ItemManager::getItemCnt(int id)
{
    return items[id].cnt;
}

const char *ItemManager::getItemName(int id)
{
    return items[id].name;
}

void ItemManager::uploadItems()
{
    DynamicJsonDocument jdoc(4096);
    // clear json
    // jdoc.clear();
    // serialize to json
    for (int i = 0; i < numItems; i++)
    {
        JsonObject obj = jdoc.createNestedObject();
        obj["id"] = i;
        // obj["name"] = items[i].name;
        // obj["phoneme"] = items[i].phoneme;
        obj["weight"] = items[i].weight;
        obj["cnt"] = items[i].cnt;
    }
    jdoc["numItems"] = numItems;
    postJson(jdoc);
}

void ItemManager::downloadItems()
{
    DynamicJsonDocument jdoc(4096);
    // clear json
    // jdoc.clear();
    getJson(jdoc);
    // get number of items(jdoc dont have numItems)
    int num = jdoc.size();
    for (int i = 0; i < num; i++)
    {
        JsonObject obj = jdoc[i];
        int id = obj["id"];
        String name = obj.containsKey("name") ? obj["name"].as<String>().c_str() : "";
        String phoneme = obj.containsKey("phoneme") ? obj["phoneme"].as<String>().c_str() : "";
        if (name.length() > 0 && phoneme.length() > 0 && strlen(items[id].phoneme) == 0)
        {
            log_i("add command %d:%s, %s\n", id, name.c_str(), phoneme.c_str());
            ESP_SR_addCommand(id, name.c_str(), phoneme.c_str());
        }
        if (id >= numItems)
        {
            addItem(name.c_str(), phoneme.c_str(), obj["weight"], obj["cnt"]);
        }
        else
        {
            strcpy(items[id].name, name.c_str());
            strcpy(items[id].phoneme, phoneme.c_str());
        }
    }
}

int ItemManager::getNumItems()
{
    return numItems;
}

ItemManager itemManager;