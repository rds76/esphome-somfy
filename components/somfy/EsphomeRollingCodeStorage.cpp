#include "EsphomeRollingCodeStorage.h"

namespace esphome {    
namespace somfy {

static const char *const TAG_CODE = "somfy";    

EsphomeRollingCodeStorage::EsphomeRollingCodeStorage(uint32_t remoteCode) {
    preferences = global_preferences->make_preference<uint16_t>(remoteCode, true);
}

uint16_t EsphomeRollingCodeStorage::nextCode() {
    uint16_t code, code_new;
    if (!preferences.load(&code)) {
        code = 1;
    }
    ESP_LOGD(TAG_CODE, "Rolling code: 0x%04X", code);        
    code_new = code + 1;
    preferences.save(&code_new);
    return code;
}

}
}