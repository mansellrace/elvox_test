#pragma once

#include <utility>
#include <vector>
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
#include "esphome/core/automation.h"


namespace esphome {
namespace elvox_intercom {

enum HardwareType {
    HW_VERSION_TYPE_2_5,
    HW_VERSION_TYPE_2_6,
    HW_VERSION_TYPE_OLDER,
};

enum LanguageType {
    LANGUAGE_DISABLED,
    LANGUAGE_PLAIN_COMMAND,
    LANGUAGE_ITALIAN,
    LANGUAGE_ENGLISH,
};

struct ElvoxIntercomData {
  std::string hex;
  std::vector<uint16_t> array;
};

class ElvoxIntercomListener {
  public:
    void set_hex(const std::string& hex) { this->hex_ = hex; }
    // void set_array(const std::vector<uint16_t> array) { this->array_ = array; }
    void set_auto_off(uint16_t auto_off) { this->auto_off_ = auto_off; }

    virtual void turn_on(uint32_t *timer, uint16_t auto_off){};
    virtual void turn_off(uint32_t *timer){};
    uint32_t timer_;
    std::string hex_;
    // std::vector<uint16_t> array_;
    uint16_t auto_off_;
};


struct ElvoxComponentStore {
  static void gpio_intr(ElvoxComponentStore *arg);

  /// Stores the time (in micros) that the leading/falling edge happened at
  ///  * An even index means a falling edge appeared at the time stored at the index
  ///  * An uneven index means a rising edge appeared at the time stored at the index
  volatile uint32_t *buffer{nullptr};
  /// The position last written to
  volatile uint32_t buffer_write_at;
  /// The position last read from
  uint32_t buffer_read_at{0};
  bool overflow{false};
  uint32_t buffer_size{400};
  uint16_t filter_us{500};
  ISRInternalGPIOPin rx_pin;
};

class ElvoxComponent : public Component {
 public:
  void elvox_decode(std::vector<uint16_t> src);
  void dump(std::vector<uint16_t>) const;
  std::string logbook_gen();
  void sending_loop();

  void set_rx_pin(InternalGPIOPin *pin) { rx_pin_ = pin; }
  void set_tx_pin(InternalGPIOPin *pin) { tx_pin_ = pin; }
  void set_hw_version(HardwareType hw_version) { hw_version_ = hw_version; }
  void set_logbook_language(LanguageType logbook_language) { logbook_language_ = logbook_language; }
  void set_logbook_entity(const char *logbook_entity) { logbook_entity_ = logbook_entity; }
  void set_sensitivity(const char *sensitivity) { sensitivity_ = sensitivity; }
  void set_buffer_size(uint32_t buffer_size) { this->buffer_size_ = buffer_size; }
  void set_filter_us(uint16_t filter_us) { this->filter_us_ = filter_us; }
  void set_idle_us(uint32_t idle_us) { this->idle_us_ = idle_us; }
  void set_dump(bool dump_raw) { this->dump_raw_ = dump_raw; }
  void set_event(const char *event) { this->event_ = event; }

  void setup() override;
  void dump_config() override;
  void loop() override;
  uint16_t command, address;
  void register_listener(ElvoxIntercomListener *listener);
  void send_command(ElvoxIntercomData data);

 protected:
  InternalGPIOPin *rx_pin_;
  InternalGPIOPin *tx_pin_;
  HardwareType hw_version_;
  LanguageType logbook_language_;
  const char* logbook_entity_;
  const char* sensitivity_;
  const char* event_;
  bool dump_raw_;
  bool simplebus_1_;
  ElvoxComponentStore store_;
  uint16_t filter_us_{10};
  uint32_t idle_us_{10000};
  uint32_t buffer_size_{};

  HighFrequencyLoopRequester high_freq_;
  std::vector<uint16_t> temp_;
  std::vector<ElvoxIntercomListener *> listeners_{};
};

template<typename... Ts> class ElvoxIntercomSendAction : public Action<Ts...> {
 public:
  // void set_hex(const std::string& hex) { this->hex_ = hex; }
  void set_array(const std::vector<uint16_t> array) { this->array_ = array; }
  // std::string hex_;
  std::vector<uint16_t> array_;

  ElvoxIntercomSendAction(ElvoxComponent *parent) : parent_(parent) {}
  TEMPLATABLE_VALUE(std::string, hex)
  // TEMPLATABLE_VALUE(std::vector<uint16_t>, array)

  void play(Ts... x) {
    ElvoxIntercomData data{};
    data.hex = this->hex_.value(x...);
    data.array = this->array_;
    this->parent_->send_command(data);
  }

 protected:
  ElvoxComponent *parent_;
};


}  // namespace elvox_intercom
}  // namespace esphome
