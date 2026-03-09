#pragma once

#include "esphome/components/light/light_output.h"

namespace esphome {
namespace somfy {

using namespace esphome::light;

class SomfyLightOutput : public LightOutput, public SomfyComponent {
protected:

public:
  LightTraits get_traits() override {
    auto traits = LightTraits();
    traits.set_supported_color_modes({ColorMode::ON_OFF});
    return traits;
  }

  void write_state(LightState *state) override {
    bool light_state;
    state->current_values_as_binary(&light_state);
    sendCC1101Command(light_state ? Command::Up : Command::Down);
  }
};

} // namespace somfy
} // namespace esphome
