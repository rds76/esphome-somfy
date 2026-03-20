#pragma once

#include "esphome/components/cover/cover.h"
#include "esphome/components/button/button.h"
#include "esphome/components/cc1101/cc1101.h"
#include "esphome/core/component.h"
#include "esphome/components/remote_transmitter/remote_transmitter.h"
#include "EsphomeRollingCodeStorage.h"

namespace esphome {
namespace somfy {

static const char *const TAG = "somfy";

enum class Command : uint8_t {
	My = 0x1,
	Up = 0x2,
	MyUp = 0x3,
	Down = 0x4,
	MyDown = 0x5,
	UpDown = 0x6,
	Prog = 0x8,
	SunFlag = 0x9,
	Flag = 0xA
};

class SomfyComponent : public Component {
protected:
  RollingCodeStorage *storage_;
  uint32_t remote_address_;
  int repeat_{4};
  remote_transmitter::RemoteTransmitterComponent *remote_transmitter_{nullptr};
  button::Button *cover_prog_button_{nullptr};

  void send_command_internal(Command command);
  void program();

  Trigger<> transmit_trigger_;
  Trigger<> complete_trigger_;

private:
  void send_command(Command command);
  void build_frame(uint8_t *frame, Command command, uint16_t code);
  void build_timings(remote_base::RawTimings & t, uint8_t *frame, uint8_t sync);
  void send_high(remote_base::RawTimings & t, int32_t durationUsecs);
  void send_low(remote_base::RawTimings & t, int32_t durationUsecs);

public:
  void setup() override;
  void dump_config() override;

  void set_remote_address(uint32_t remote_address) { this->remote_address_ = remote_address; }
  void set_repeat(int repeat) { this->repeat_ = repeat; }
  void set_remote_transmitter(remote_transmitter::RemoteTransmitterComponent *t) { this->remote_transmitter_ = t; }
  void set_prog_button(button::Button *cover_prog_button) { this->cover_prog_button_ = cover_prog_button; }

  Trigger<> *get_transmit_trigger() { return &this->transmit_trigger_; }
  Trigger<> *get_complete_trigger() { return &this->complete_trigger_; }
};

} // namespace somfy
} // namespace esphome
