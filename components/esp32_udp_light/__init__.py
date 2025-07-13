import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light
from esphome.components.light import AddressableLightState
from esphome.const import CONF_ID, CONF_PORT, CONF_LIGHT_ID

DEPENDENCIES = ['network']
AUTO_LOAD = ['light']

esp32_udp_light_ns = cg.esphome_ns.namespace('esp32_udp_light')
UDPStripLightComponent = esp32_udp_light_ns.class_('UDPStripLightComponent', cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(UDPStripLightComponent),
    cv.Required(CONF_PORT): cv.port,
    cv.Required(CONF_LIGHT_ID): cv.use_id(AddressableLightState),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    light_state = await cg.get_variable(config[CONF_LIGHT_ID])
    cg.add(var.set_strip(light_state))
    cg.add(var.set_port(config[CONF_PORT]))