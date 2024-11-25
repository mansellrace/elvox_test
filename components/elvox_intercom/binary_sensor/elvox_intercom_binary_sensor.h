#pragma once
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
#include "../elvox_intercom.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome {
namespace elvox_intercom {

class ElvoxIntercomBinarySensor : public binary_sensor::BinarySensor, public ElvoxIntercomListener  {
  public:
    void turn_on(uint32_t *timer, uint16_t auto_off) override;
    void turn_off(uint32_t *timer) override;

  protected:
    binary_sensor::BinarySensor *incoming_hex_sensor_{nullptr};
    uint16_t hex_;
};

}  // namespace elvox_intercom
}  // namespace esphome
