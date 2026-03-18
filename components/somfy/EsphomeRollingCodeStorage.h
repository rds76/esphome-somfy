#pragma once

#include "RollingCodeStorage.h"
#include "esphome.h"

namespace esphome {
namespace somfy {

class EsphomeRollingCodeStorage : public RollingCodeStorage {
private:
    ESPPreferenceObject preferences;

public:
    EsphomeRollingCodeStorage(uint32_t remoteCode);
    uint16_t nextCode() override;
};

} // namespace somfy
} // namespace esphome
