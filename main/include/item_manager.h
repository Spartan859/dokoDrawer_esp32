#include "esp32-hal-sr.h"
#ifndef ITEM_MANAGER_H
#define ITEM_MANAGER_H

// items[]: id-> name, phoneme, weight, cnt
struct Item
{
    int id;
    char name[SR_CMD_STR_LEN_MAX];
    char phoneme[SR_CMD_PHONEME_LEN_MAX];
    int weight;
    int cnt;
};
class ItemManager
{
public:
    ItemManager();
    ~ItemManager();
    void addItemCnt(int id, int cnt);                                         // add item count
    int searchItem(int wt);                                                   // search item by weight
    void setItem(int id, char *name, char *phoneme);                          // set item
    void addItem(const char *name, const char *phoneme, int weight, int cnt); // add item
    void setWtThreshold(int wtThr);                                           // set weight threshold
    int getItemCnt(int id);                                                   // get item count
    const char *getItemName(int id);                                          // get item name
    void downloadItems();                                                     // download items from server
    int getNumItems();                                                        // get number of items

private:
    void uploadItems(); // upload items to server
    Item items[105];
    int numItems;
    int wt_threshold;
};

extern ItemManager itemManager;

#endif // ITEM_MANAGER_H