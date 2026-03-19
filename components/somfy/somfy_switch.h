#pragma once

#include "esphome/components/switch/light_output.h"

namespace esphome {
namespace somfy {

using namespace esphome::switch_;

class SomfySwitch : public Switch, public SomfyComponent {
protected:
  
public:

  void write_state(bool state) override {    
    sendCC1101Command(state ? Command::Up : Command::Down);
  }

  void dump_config() override {
    ESP_LOGCONFIG(TAG, "Somfy switch:\n");
    SomfyComponent::dump_config();
  }
};

} // namespace somfy
} // namespace esphome
