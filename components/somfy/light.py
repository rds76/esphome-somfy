import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import light, cc1101
from esphome import pins

DEPENDENCIES = ["cc1101"]

somfy_ns = cg.esphome_ns.namespace("somfy")
SomfyLightOutput = somfy_ns.class_("SomfyLightOutput", light.LightOutput, cg.Component)

CONF_SOMFY_REMOTE_ADDRESS = "remote_address"
CONF_SOMFY_PIN = "pin"
CONF_SOMFY_RF_FREQ = "rf_freq"
CONF_SOMFY_SOMFY_FREQ = "somfy_freq"
CONF_SOMFY_REPEAT = "repeat"
CONF_SOMFY_CC1101 = "cc1101"

CONFIG_SCHEMA = light.light_schema(SomfyLightOutput, light.LightType.BINARY).extend(
    {
        cv.Required(CONF_SOMFY_PIN): pins.internal_gpio_output_pin_schema,
        cv.Required(CONF_SOMFY_REMOTE_ADDRESS): cv.int_range(min=0, max=0xFFFFFFFF),        
        cv.Optional(CONF_SOMFY_REPEAT, default=1): cv.int_range(min=1, max=16),
        cv.Optional(CONF_SOMFY_RF_FREQ, default="433.92MHz"): cv.All(
          cv.frequency, cv.float_range(min=300.0e6, max=928.0e6)
        ),
        cv.Optional(CONF_SOMFY_SOMFY_FREQ, default="433.42MHz"): cv.All(
          cv.frequency, cv.float_range(min=300.0e6, max=928.0e6)
        ),        
        cv.Required(CONF_SOMFY_CC1101): cv.use_id(cc1101.CC1101Component),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = await light.new_light(config)
    await cg.register_component(var, config)

    pin = await cg.gpio_pin_expression(config[CONF_SOMFY_PIN])
    cg.add(var.set_pin(pin))

    cc1101 = await cg.get_variable(config[CONF_SOMFY_CC1101])
    cg.add(var.set_cc1101(cc1101))

    cg.add(var.set_remote_address(config[CONF_SOMFY_REMOTE_ADDRESS]))
    cg.add(var.set_rf_freq(config[CONF_SOMFY_RF_FREQ]))
    cg.add(var.set_somfy_freq(config[CONF_SOMFY_SOMFY_FREQ]))
    cg.add(var.set_repeat(config[CONF_SOMFY_REPEAT]))
