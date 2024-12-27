#include "SR_MY.h"

#include "I2S_MY.h"
#include "ESP_I2S.h"
#include "ESP_SR.h"
#include "esp32-hal-sr.h"
#include "TTS_MY.h"
#include "item_manager.h"

enum
{
    SR_CMD_HELLO = 100000,
};
int sr_commands_len = 0;
sr_cmd_t sr_commands[105] = {
    // {0, "Turn on the light", "TkN nN jc LiT"},
    // {0, "你好", "ni hao"},
};

void onSrEvent(sr_event_t event, int command_id, int phrase_id)
{
    switch (event)
    {
    case SR_EVENT_WAKEWORD:
        Serial.println("WakeWord Detected!");
        break;
    case SR_EVENT_WAKEWORD_CHANNEL:
        Serial.printf("WakeWord Channel %d Verified!\n", command_id);
        ESP_SR.setMode(SR_MODE_COMMAND); // Switch to Command detection
        break;
    case SR_EVENT_TIMEOUT:
        Serial.println("Timeout Detected!");
        ESP_SR.setMode(SR_MODE_WAKEWORD); // Switch back to WakeWord detection
        break;
    case SR_EVENT_COMMAND:
        Serial.printf("Command %d Detected!\n", command_id);
        if (command_id < itemManager.getNumItems())
        {
            if (itemManager.getItemCnt(command_id) > 0)
            {
                tts_speak("抽屉里有", i2s_out);
            }
            else
            {
                tts_speak("抽屉里没有", i2s_out);
            }
            tts_speak(itemManager.getItemName(command_id), i2s_out);
        }
        ESP_SR.setMode(SR_MODE_COMMAND); // Allow for more commands to be given, before timeout
        // ESP_SR.setMode(SR_MODE_WAKEWORD); // Switch back to WakeWord detection
        break;
    default:
        Serial.println("Unknown Event!");
        break;
    }
}

void ESP_SR_init()
{
    ESP_SR.onEvent(onSrEvent);
    ESP_SR.begin(i2s, sr_commands, sizeof(sr_commands) / sizeof(sr_cmd_t), SR_CHANNELS_MONO, SR_MODE_WAKEWORD);
}

void ESP_SR_addCommand(int id, const char *str, const char *phoneme)
{
    sr_cmd_t sr_command;
    sr_command.command_id = id;
    strncpy(sr_command.str, str, SR_CMD_STR_LEN_MAX);
    strncpy(sr_command.phoneme, phoneme, SR_CMD_PHONEME_LEN_MAX);
    sr_commands[sr_commands_len++] = sr_command;
    ESP_SR.end();
    // ESP_SR.addCommand(sr_command);
    ESP_SR.begin(i2s, sr_commands, sr_commands_len, SR_CHANNELS_MONO, SR_MODE_WAKEWORD);
}