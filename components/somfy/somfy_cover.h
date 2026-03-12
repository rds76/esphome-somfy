#pragma once

#include "esphome/components/cover/cover.h"

namespace esphome {
namespace somfy {

using namespace esphome::cover;

class SomfyCover : public Cover, public SomfyComponent {
protected:
 
public:
  CoverTraits get_traits() override {
    auto traits = CoverTraits();
    traits.set_is_assumed_state(true);
    traits.set_supports_position(false);
    traits.set_supports_tilt(false);
    traits.set_supports_stop(true);
    return traits;
  }

  void control(const CoverCall &call) override {
    if (call.get_position().has_value()) {
      float pos = *call.get_position();

      if (pos == COVER_OPEN) {
        ESP_LOGI(TAG, "OPEN");
        sendCC1101Command(Command::Up);
      } else if (pos == COVER_CLOSED) {
        ESP_LOGI(TAG, "CLOSE");
        sendCC1101Command(Command::Down);
      } else {
        ESP_LOGI(TAG, "WAT");
      }

      this->position = pos;
      this->publish_state();
    }

    if (call.get_stop()) {
      ESP_LOGI(TAG, "STOP");
      sendCC1101Command(Command::My);
    }
  }

  void dump_config() override {
    ESP_LOGCONFIG(TAG, "Somfy cover:\n");
    SomfyComponent::dump_config();
  }
};

} // namespace somfy
} // namespace esphome
