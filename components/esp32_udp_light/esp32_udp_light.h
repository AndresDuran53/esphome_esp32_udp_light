#pragma once

#include "esphome/core/component.h"
#include "esphome/components/light/addressable_light.h"

namespace esphome {
namespace esp32_udp_light {

class UDPStripLightComponent : public Component {
  public:
    UDPStripLightComponent();
    // Indicates if the UDP effect is active
    bool is_effect_active = false;

  // Boot counter to delay socket init
  int boot_loop_counter_ = 0;
  static constexpr int BOOT_LOOP_DELAY = 200; // Ajusta según necesidad
    
    void setup() override;
    void loop() override;
    void dump_config() override;
    // ~UDPStripLightComponent();

    void set_strip(light::AddressableLightState* light_strip) { this->light_strip_ = light_strip; }
    void set_port(uint16_t port) { this->port_ = port; }
    void set_effect_active(bool active) { this->is_effect_active = active; }
    void set_socket_fd(int socket_fd) { this->socket_fd_ = socket_fd; }

  protected:
    // Internal members
    light::AddressableLightState* light_strip_{nullptr};
    int socket_fd_{-1};
    uint16_t port_{0};

    // Opens the UDP socket
    void open_udp_socket_();
    // Receives UDP data and validates errors
    bool receive_udp_data_(uint8_t* buffer, size_t buffer_size, int* received_bytes);
    // Updates the LEDs with the received UDP data
    void update_leds_from_udp_(light::AddressableLight* addressable_light, const uint8_t* buffer, int received_bytes);
    // Returns the AddressableLight pointer or nullptr if not available
    light::AddressableLight* get_addressable_light_();
};


}  // namespace esp32_udp_light
}  // namespace esphome