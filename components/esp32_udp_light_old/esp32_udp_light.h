#pragma once

#include "esphome/core/component.h"
#include "esphome/components/light/addressable_light.h"
#include "esphome/components/light/light_state.h"
#include "esphome/components/network/util.h"
#include <vector>

namespace esphome {
namespace esp32_udp_light {

class UDPStripLightComponent : public Component {
 public:
  // Indicates if the UDP effect is active
  bool is_effect_active = false;

  // LED strip and port configuration
  void set_strip(light::AddressableLightState* light_strip) { this->light_strip_ = light_strip; }
  void set_port(uint16_t port) { this->port_ = port; }
  void set_effect_active(bool active) { this->is_effect_active = active; }

  // Lifecycle methods
  void setup() override;
  void loop() override;
  ~UDPStripLightComponent();

 protected:
  // Opens the UDP socket
  void open_udp_socket_();
  // Receives UDP data and validates errors
  bool receive_udp_data_(uint8_t* buffer, size_t buffer_size, int* received_bytes);
  // Updates the LEDs with the received UDP data
  void update_leds_from_udp_(light::AddressableLight* addressable_light, const uint8_t* buffer, int received_bytes);
  // Returns the AddressableLight pointer or nullptr if not available
  light::AddressableLight* get_addressable_light_();

  // Internal members
  light::AddressableLightState* light_strip_{nullptr};
  int socket_fd_{-1};
  uint16_t port_{0};
};

}  // namespace esp32_udp_light
}  // namespace esphome
