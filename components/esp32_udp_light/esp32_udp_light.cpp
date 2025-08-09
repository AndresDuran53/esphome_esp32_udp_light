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
    ESP_LOGI(TAG, "UDPStripLightComponent loop() called");
    // If the socket is not open, try to open it
    if (this->socket_fd_ < 0) {
        this->open_udp_socket_();
    }
    if (this->socket_fd_ < 0){
        ESP_LOGE(TAG, "Socket not open, cannot receive data");
        return;
    }
}

void UDPStripLightComponent::dump_config(){
    ESP_LOGCONFIG(TAG, "UDPStripLightComponent: ");
    ESP_LOGCONFIG(TAG, "  Port: %d", this->port_);
    ESP_LOGI(TAG, "UDPStripLightComponent LogInfo");
}

/*
UDPStripLightComponent::~UDPStripLightComponent() {
  if (this->socket_fd_ >= 0) {
    close(this->socket_fd_);
    this->socket_fd_ = -1;
  }
}*/

void UDPStripLightComponent::open_udp_socket_(){
    if (this->socket_fd_ >= 0) {
        close(this->socket_fd_);
        this->socket_fd_ = -1;
    }
    ESP_LOGI(TAG, "Setting up UDP listener on port %d", this->port_);
    this->socket_fd_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (this->socket_fd_ < 0) {
        ESP_LOGE(TAG, "Failed to create socket: errno %d", errno);
        return;
    }
    struct sockaddr_in listen_address;
    listen_address.sin_addr.s_addr = htonl(INADDR_ANY);
    listen_address.sin_family = AF_INET;
    listen_address.sin_port = htons(this->port_);
    if (bind(this->socket_fd_, (struct sockaddr*)&listen_address, sizeof(listen_address)) < 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        close(this->socket_fd_);
        this->socket_fd_ = -1;
        return;
    }
    fcntl(this->socket_fd_, F_SETFL, O_NONBLOCK);
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
    int led_count = addressable_light->size();
    int leds_received = received_bytes / 3;
    int leds_to_update = std::min(led_count, leds_received);

    // Update LEDs with received RGB data
    for (int led_index = 0; led_index < leds_to_update; led_index++) {
        int buffer_position = led_index * 3;
        auto pixel = addressable_light->get(led_index);
        pixel.set(Color(
        buffer[buffer_position],
        buffer[buffer_position + 1],
        buffer[buffer_position + 2]
        ));
    }
    // Schedule LED strip update
    addressable_light->schedule_show();
}

light::AddressableLight* UDPStripLightComponent::get_addressable_light_() {
    if (!this->light_strip_) return nullptr;
    auto light_output = this->light_strip_->get_output();
    if (light_output == nullptr) return nullptr;
    return static_cast<light::AddressableLight*>(light_output);
}

}  // namespace esp32_udp_light
}  // namespace esphome