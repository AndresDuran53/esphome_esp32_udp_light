#include "esphome/core/log.h"
#include "esp32_udp_light.h"

namespace esphome {
namespace esp32_udp_light {

static const char *TAG = "esp32_udp_light.component";

void UDPLightStrip::setup() {

}

void UDPLightStrip::loop() {

}

void UDPLightStrip::dump_config(){
    ESP_LOGCONFIG(TAG, "UDPLightStrip esp32_udp_light");
}


}  // namespace esp32_udp_light
}  // namespace esphome