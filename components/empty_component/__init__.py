import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light
from esphome.components.light import AddressableLightState
from esphome.const import CONF_ID, CONF_PORT, CONF_LIGHT_ID

DEPENDENCIES = ['wifi']
AUTO_LOAD = ['light']

empty_component_ns = cg.esphome_ns.namespace("empty_component")
EmptyComponent = empty_component_ns.class_("EmptyComponent", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(EmptyComponent),
        cv.Required(CONF_PORT): cv.port,
        cv.Required(CONF_LIGHT_ID): cv.use_id(AddressableLightState),
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    light_state = await cg.get_variable(config[CONF_LIGHT_ID])
    cg.add(var.set_strip(light_state))
    cg.add(var.set_port(config[CONF_PORT]))