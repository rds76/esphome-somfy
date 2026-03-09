#pragma once

#include "esphome/components/cover/cover.h"
#include "esphome/components/cc1101/cc1101.h"
#include "esphome/core/component.h"
#include "SomfyRemote.h"
#include "EsphomeRollingCodeStorage.h"

namespace esphome {
namespace somfy {

using namespace esphome::cover;

static const char *const TAG = "somfy";

class SomfyComponent : public Component {
protected:
  SomfyRemote *remote_;
  RollingCodeStorage *storage_;
  const char *storage_namespace_;
  const char *storage_key_;
  InternalGPIOPin *emitter_pin_;
  uint32_t remote_address_;
  int repeat_;
  cc1101::CC1101Component *cc1101_;
  float rf_freq_, somfy_freq_;

public:
  void setup() override {
    this->emitter_pin_->pin_mode(gpio::FLAG_OUTPUT);
    this->emitter_pin_->digital_write(false);

    //storage_ = new NVSRollingCodeStorage(storage_namespace_, storage_key_);
    storage_ = new EsphomeRollingCodeStorage(remote_address_);
    remote_ = new SomfyRemote(emitter_pin_, remote_address_, storage_);
  }

  void sendCC1101Command(Command command) {
    ESP_LOGD(TAG, "Entering TX with freq:: %.0fHz", this->somfy_freq_);     
    cc1101_->set_idle();
    cc1101_->set_frequency(this->somfy_freq_);
    cc1101_->begin_tx();
    ESP_LOGD(TAG, "Sending %dx command: 0x%x", this->repeat_, command);     
    remote_->sendCommand(command, this->repeat_);
    ESP_LOGD(TAG, "Entering RX with freq:: %.0fHz", this->rf_freq_);     
    cc1101_->set_idle();
    cc1101_->set_frequency(this->rf_freq_);
    cc1101_->begin_rx();
  }

  void program() {
    ESP_LOGI(TAG, "PROG");
    sendCC1101Command(Command::Prog);
  }

  void set_pin(InternalGPIOPin *pin) { this->emitter_pin_ = pin; }

  void set_remote_address(uint32_t remote_address) { this->remote_address_ = remote_address; }
  void set_storage_namespace(const char *storage_namespace) {
    this->storage_namespace_ = storage_namespace;
  }
  void set_storage_key(const char *storage_key) {
    this->storage_key_ = storage_key;
  }
  void set_repeat(int repeat) { this->repeat_ = repeat; }
  void set_cc1101(cc1101::CC1101Component *cc1101) { this->cc1101_ = cc1101; }
  void set_rf_freq(float freq) { this->rf_freq_ = freq; }
  void set_somfy_freq(float freq) { this->somfy_freq_ = freq; }
};

} // namespace somfy
} // namespace esphome
