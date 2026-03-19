#pragma once

#include "esphome/components/switch/switch.h"

namespace esphome {
namespace somfy {

using namespace esphome::switch_;

class SomfySwitch : public Switch, public SomfyComponent {
protected:
  
public:

  void write_state(bool state) override {    
    send_command(state ? Command::Up : Command::Down);
    this->publish_state(state);
  }

  void dump_config() override {
    ESP_LOGCONFIG(TAG, "Somfy switch:\n");
    SomfyComponent::dump_config();
  }

  bool assumed_state() override { return false; }
};

} // namespace somfy
} // namespace esphome
