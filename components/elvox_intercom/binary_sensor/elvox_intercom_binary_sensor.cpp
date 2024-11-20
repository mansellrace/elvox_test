#include "elvox_intercom_binary_sensor.h"

namespace esphome {
namespace elvox_intercom {

static const char *const TAG = "elvox.binary";

void ElvoxIntercomBinarySensor::turn_on(uint32_t *timer, uint16_t auto_off ) {
  this->publish_state(true);
  if (auto_off > 0) *timer = millis() + (auto_off * 1000);
}

void ElvoxIntercomBinarySensor::turn_off(uint32_t *timer) {
  this->publish_state(false);
  *timer = 0;
}


}  // namespace elvox_intercom
}  // namespace esphome
