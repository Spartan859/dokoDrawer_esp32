#ifndef MY_SPIFFS_H
#define MY_SPIFFS_H

#include <esp_err.h>
#include <esp_spiffs.h>

#define BSP_SPIFFS_MOUNT_POINT "/spiffs"
#define BSP_SPIFFS_PARTITION_LABEL "storage"
#define BSP_SPIFFS_MAX_FILES 5

esp_err_t bsp_spiffs_mount(void);

#endif // MY_SPIFFS_H