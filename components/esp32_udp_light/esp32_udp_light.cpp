#include "esphome/core/log.h"
#include "esp32_udp_light.h"

namespace esphome {
namespace esp32_udp_light {

static const char *TAG = "esp32_udp_light.component";

void UDPStripLightComponent::setup() {

}

void UDPStripLightComponent::loop() {

}

void UDPStripLightComponent::dump_config(){
    ESP_LOGCONFIG(TAG, "UDPStripLightComponent esp32_udp_light");
}

UDPStripLightComponent::~UDPStripLightComponent() {
  if (this->socket_fd_ >= 0) {
    close(this->socket_fd_);
    this->socket_fd_ = -1;
  }
}

void open_udp_socket_(){

}

bool receive_udp_data_(uint8_t* buffer, size_t buffer_size, int* received_bytes){

}

void update_leds_from_udp_(light::AddressableLight* addressable_light, const uint8_t* buffer, int received_bytes){

}

light::AddressableLight* get_addressable_light_() {
    
}

}  // namespace esp32_udp_light
}  // namespace esphome