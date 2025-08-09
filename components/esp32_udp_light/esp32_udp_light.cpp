#include "esphome/core/log.h"
#include "esp32_udp_light.h"
#include <lwip/sockets.h>
#include "esphome/components/wifi/wifi_component.h"

namespace esphome {

namespace esp32_udp_light {


// Constructor: initializes members
UDPStripLightComponent::UDPStripLightComponent()
    : is_effect_active(false), boot_loop_counter_(0), socket_fd_(-1), port_(0), light_strip_(nullptr) {}

static const char *TAG = "esp32_udp_light.component";

// Constant value for UDP buffer size
constexpr size_t UDP_BUFFER_SIZE = 2048;


// Initial component setup
void UDPStripLightComponent::setup() {
    ESP_LOGI(TAG, "UDPStripLightComponent setup() called");
}


// Main loop: handles UDP reception and LED update
void UDPStripLightComponent::loop() {
    if (!wifi::global_wifi_component || !wifi::global_wifi_component->is_connected()) {
    ESP_LOGI(TAG, "Waiting for WiFi...");
        return;
    }

    if (this->boot_loop_counter_ < BOOT_LOOP_DELAY) {
        this->boot_loop_counter_++;
        if (this->boot_loop_counter_ == BOOT_LOOP_DELAY) {
            ESP_LOGI(TAG, "Boot loop counter reached %d, initializing socket", BOOT_LOOP_DELAY);
            this->boot_loop_counter_++;
        }
        return;
    }

    if (this->socket_fd_ < 0) {
        this->open_udp_socket_();
    }
    if (this->socket_fd_ < 0) {
        ESP_LOGE(TAG, "Socket not open, cannot receive data");
        return;
    }

    // Get the AddressableLight object
    auto addressable_light = this->get_addressable_light_();
    if (addressable_light == nullptr) return;

    // Receive and process UDP data
    int received_bytes = 0;
    uint8_t udp_buffer[UDP_BUFFER_SIZE];
    if (!this->receive_udp_data_(udp_buffer, sizeof(udp_buffer), &received_bytes)) return;

    // Check if the strip is ON
    if (!this->light_strip_->current_values.is_on()) return;

    // Check if the current effect is UDP
    std::string current_effect = this->light_strip_->get_effect_name();
    if (current_effect != "WLED UDP Effect") {
        this->is_effect_active = false;
        return;
    }

    // Update LEDs with received data
    this->update_leds_from_udp_(addressable_light, udp_buffer, received_bytes);
}


// Show the component configuration in the log
void UDPStripLightComponent::dump_config() {
    ESP_LOGCONFIG(TAG, "UDPStripLightComponent: ");
    ESP_LOGCONFIG(TAG, "  Port: %d", this->port_);
    ESP_LOGCONFIG(TAG, "  socket_fd_: %d", this->socket_fd_);
}


// Destructor: closes the socket if it is open
UDPStripLightComponent::~UDPStripLightComponent() {
    if (this->socket_fd_ >= 0) {
        close(this->socket_fd_);
        this->socket_fd_ = -1;
    }
}


// Opens the UDP socket and sets it to non-blocking
void UDPStripLightComponent::open_udp_socket_() {
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


// Receives UDP data in a non-blocking way
bool UDPStripLightComponent::receive_udp_data_(uint8_t* buffer, size_t buffer_size, int* received_bytes) {
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


// Updates the LEDs with the data received via UDP
void UDPStripLightComponent::update_leds_from_udp_(light::AddressableLight* addressable_light, const uint8_t* buffer, int received_bytes) {
    int led_count = addressable_light->size();
    int leds_received = received_bytes / 3;
    int leds_to_update = std::min(led_count, leds_received);

    // Log RGB values
    for (int led_index = 0; led_index < leds_to_update; led_index++) {
        int buffer_position = led_index * 3;
        uint8_t r = buffer[buffer_position];
        uint8_t g = buffer[buffer_position + 1];
        uint8_t b = buffer[buffer_position + 2];
    // Uncomment the following line only for debugging
    // ESP_LOGI(TAG, "LED %d: R=%d G=%d B=%d", led_index, r, g, b);
        auto pixel = addressable_light->get(led_index);
        pixel.set(Color(r, g, b));
    }
    addressable_light->schedule_show();
}


// Returns the AddressableLight pointer or nullptr if not available
light::AddressableLight* UDPStripLightComponent::get_addressable_light_() {
    if (!this->light_strip_) return nullptr;
    auto light_output = this->light_strip_->get_output();
    if (light_output == nullptr) return nullptr;
    return static_cast<light::AddressableLight*>(light_output);
}

}  // namespace esp32_udp_light
}  // namespace esphome