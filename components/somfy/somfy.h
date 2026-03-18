#pragma once

#include "esphome/components/cover/cover.h"
#include "esphome/components/button/button.h"
#include "esphome/components/cc1101/cc1101.h"
#include "esphome/core/component.h"
#include "esphome/components/remote_transmitter/remote_transmitter.h"
#include "EsphomeRollingCodeStorage.h"

namespace esphome {
namespace somfy {

using namespace esphome::cover;

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
  cc1101::CC1101Component *cc1101_;
  float rf_freq_, somfy_freq_;
  remote_transmitter::RemoteTransmitterComponent *remote_transmitter_{nullptr};
  button::Button *cover_prog_button_{nullptr};

  void send_command(Command command) {
    const uint16_t rollingCode = this->storage_->nextCode();
    uint8_t frame[7];
    build_frame(frame, command, rollingCode);
    remote_base::RawTimings t;
    build_timings(t, frame, 2);
    for (int i = 0; i < this->repeat_; i++) {
      build_timings(t, frame, 7);
    }
    auto call = this->remote_transmitter_->transmit();
    call.get_data()->set_data(t);
    call.perform();
  }

  void build_frame(uint8_t *frame, Command command, uint16_t code) {
    const uint8_t button = static_cast<uint8_t>(command);
    frame[0] = 0xA7;          // Encryption key. Doesn't matter much
    frame[1] = button << 4;   // Which button did  you press? The 4 LSB will be the checksum
    frame[2] = code >> 8;     // Rolling code (big endian)
    frame[3] = code;          // Rolling code

    frame[4] = this->remote_address_ >> 16;  // Remote address
    frame[5] = this->remote_address_ >> 8;   // Remote address
    frame[6] = this->remote_address_;        // Remote address

    // Checksum calculation: a XOR of all the nibbles
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < 7; i++) {
      checksum = checksum ^ frame[i] ^ (frame[i] >> 4);
    }
    checksum &= 0b1111;  // We keep the last 4 bits only

    // Checksum integration
    frame[1] |= checksum;

    // Obfuscation: a XOR of all the bytes
    for (uint8_t i = 1; i < 7; i++) {
      frame[i] ^= frame[i - 1];
    }
  }

  void build_timings(remote_base::RawTimings & t, uint8_t *frame, uint8_t sync) {
    const int32_t SYMBOL = 640;

    if (sync == 2) {  // Only with the first frame.
      // Wake-up pulse & Silence
      send_high(t, 9415);
      send_low(t, 9565 + 80000); // was delay(80)
    }

    // Hardware sync: two sync for the first frame, seven for the following ones.
    for (int i = 0; i < sync; i++) {
      send_high(t, 4 * SYMBOL);
      send_low(t, 4 * SYMBOL);
    }

    // Software sync
    send_high(t, 4550);
    send_low(t, SYMBOL);

    // Data: bits are sent one by one, starting with the MSB.
    for (uint8_t i = 0; i < 56; i++) {
      if (((frame[i / 8] >> (7 - (i % 8))) & 1) == 1) {
        send_low(t, SYMBOL);
        send_high(t, SYMBOL);
      } else {
        send_high(t, SYMBOL);
        send_low(t, SYMBOL);
      }
    }

    // Inter-frame silence
    send_low(t, 415 + 30000); // was delay(30)
  }

  void send_high(remote_base::RawTimings & t, int32_t durationUsecs) {
    t.push_back(static_cast<int32_t>(durationUsecs));
  }

  void send_low(remote_base::RawTimings & t, int32_t durationUsecs) {
    t.push_back(-static_cast<int32_t>(durationUsecs));
  }

  void sendCC1101Command(Command command) {    
    change_freq(this->somfy_freq_, 10);
    ESP_LOGD(TAG, "Sending %dx command: 0x%x for button addr: 0x%x", this->repeat_, command, this->remote_address_);
    send_command(command);
    ESP_LOGD(TAG, "Setting freq to %.0fHz", this->rf_freq_);
    change_freq(this->rf_freq_, 0);
  }

  void program() {
    ESP_LOGI(TAG, "PROG");
    sendCC1101Command(Command::Prog);
  }

  void change_freq(float f, uint32_t delay_ms) {
    if (this->rf_freq_ == this->somfy_freq_) return;
    ESP_LOGD(TAG, "Setting freq to %.0fHz", f);
    cc1101_->set_idle();
    cc1101_->set_frequency(f);
    if (delay_ms > 0) delay(delay_ms);
  }

public:
  void setup() override {
    storage_ = new EsphomeRollingCodeStorage(remote_address_);
    // Attach the prog button
    if (this->cover_prog_button_) {
      this->cover_prog_button_->add_on_press_callback(
        [=, this] { return this->program(); });
    }
  }

  void dump_config() override {
    ESP_LOGCONFIG(TAG,
                  "  Remote address: 0x%x\n"
                  "  Repeat command: %dx\n"
                  "  Tx frequency: %.0f Hz\n"
                  "  Rx frequency (RF): %.0f Hz\n",
                  this->remote_address_, this->repeat_, this->somfy_freq_, this->rf_freq_);
}

  void set_remote_address(uint32_t remote_address) { this->remote_address_ = remote_address; }
  void set_repeat(int repeat) { this->repeat_ = repeat; }
  void set_cc1101(cc1101::CC1101Component *cc1101) { this->cc1101_ = cc1101; }
  void set_rf_freq(float freq) { this->rf_freq_ = freq; }
  void set_somfy_freq(float freq) { this->somfy_freq_ = freq; }
  void set_remote_transmitter(remote_transmitter::RemoteTransmitterComponent *t) { this->remote_transmitter_ = t; }
  void set_prog_button(button::Button *cover_prog_button) { this->cover_prog_button_ = cover_prog_button; }
};

} // namespace somfy
} // namespace esphome
