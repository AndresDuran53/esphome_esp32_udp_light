#pragma once

#include "esphome/core/component.h"
#include "esphome/components/light/addressable_light.h"

namespace esphome {
namespace esp32_udp_light {

class EmptyComponent : public Component {
  public:
    void setup() override;
    void loop() override;
    void dump_config() override;

    void set_strip(light::AddressableLightState* light_strip) { this->light_strip_ = light_strip; }
    void set_port(uint16_t port) { this->port_ = port; }

  protected:
    // Internal members
    light::AddressableLightState* light_strip_{nullptr};
    int socket_fd_{-1};
    uint16_t port_{0};
};


}  // namespace esp32_udp_light
}  // namespace esphome