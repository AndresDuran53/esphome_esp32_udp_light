#include "esphome/core/log.h"
#include "esp32_udp_light.h"

namespace esphome {
namespace esp32_udp_light {

static const char *TAG = "esp32_udp_light.component";

void EmptyComponent::setup() {

}

void EmptyComponent::loop() {

}

void EmptyComponent::dump_config(){
    ESP_LOGCONFIG(TAG, "Empty component esp32_udp_light");
}


}  // namespace esp32_udp_light
}  // namespace esphome