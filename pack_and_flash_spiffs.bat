python C:\Users\qianj\esp\v5.1.4\esp-idf\components\spiffs\spiffsgen.py 0x100000 filesystem spiffs.bin

python C:\Users\qianj\esp\v5.1.4\esp-idf\components\esptool_py\esptool\esptool.py --chip esp32s3 --port COM7 --baud 460800 write_flash -z 0x610000 spiffs.bin