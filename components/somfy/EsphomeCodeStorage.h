#pragma once

#include "esphome.h"

namespace esphome {
namespace somfy {

static const char *const TAG = "somfy.code_storage";

class EsphomeCodeStorage : public RollingCodeStorage {
private:
    ESPPreferenceObject preferences;

public:
    EsphomeCodeStorage(uint32_t remoteCode)
    {
        preferences = global_preferences->make_preference<uint16_t>(remoteCode, true);
    }
    uint16_t nextCode() override
    {
        uint16_t code, code_new;
        preferences.load(&code);        
        ESP_LOGD(TAG, "Rolling code: %04X", code);        
        code_new = code + 1;
        preferences.save(&code_new);
        return code;
    }
};

} // namespace somfy
} // namespace esphome
