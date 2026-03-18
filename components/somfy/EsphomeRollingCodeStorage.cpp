#include "EsphomeRollingCodeStorage.h"

namespace esphome {    
namespace somfy {

EsphomeRollingCodeStorage::EsphomeRollingCodeStorage(uint32_t remoteCode) {
    preferences = global_preferences->make_preference<uint16_t>(remoteCode, true);
}

uint16_t EsphomeRollingCodeStorage::nextCode() {
    uint16_t code, code_new;
    if (!preferences.load(&code)) {
        code = 1;
    }    
    code_new = code + 1;
    preferences.save(&code_new);
    return code;
}

}
}