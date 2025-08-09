#include "esphome/core/log.h"
#include "esp32_udp_light.h"
#include <lwip/sockets.h>

namespace esphome {
namespace esp32_udp_light {

static const char *TAG = "esp32_udp_light.component";

void UDPStripLightComponent::setup() {
    ESP_LOGI(TAG, "UDPStripLightComponent setup() called");
}

void UDPStripLightComponent::loop() {

}

void UDPStripLightComponent::dump_config(){
    ESP_LOGCONFIG(TAG, "UDPStripLightComponent: ");
    ESP_LOGI(TAG, "UDPStripLightComponent LogInfo");
}

UDPStripLightComponent::~UDPStripLightComponent() {
  if (this->socket_fd_ >= 0) {
    close(this->socket_fd_);
    this->socket_fd_ = -1;
  }
}

void UDPStripLightComponent::open_udp_socket_(){

}

bool UDPStripLightComponent::receive_udp_data_(uint8_t* buffer, size_t buffer_size, int* received_bytes){
    struct sockaddr_in sender_address;
    socklen_t address_length = sizeof(sender_address);
    *received_bytes = recvfrom(this->socket_fd_, buffer, buffer_size, 0,
                                (struct sockaddr*)&sender_address, &address_length);
    if (*received_bytes < 0 && errno != EWOULDBLOCK && errno != EAGAIN) {
        ESP_LOGE(TAG, "Socket error on recvfrom: errno %d", errno);
        close(this->socket_fd_);
        this->socket_fd_ = -1;
        return false;
    }
    if (*received_bytes <= 0) return false;
    return true;
}

void UDPStripLightComponent::update_leds_from_udp_(light::AddressableLight* addressable_light, const uint8_t* buffer, int received_bytes){

}

light::AddressableLight* UDPStripLightComponent::get_addressable_light_() {
    if (!this->light_strip_) return nullptr;
    auto light_output = this->light_strip_->get_output();
    if (light_output == nullptr) return nullptr;
    return static_cast<light::AddressableLight*>(light_output);
}

}  // namespace esp32_udp_light
}  // namespace esphome