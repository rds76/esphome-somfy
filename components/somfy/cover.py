import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.components import cover, cc1101, remote_transmitter
from esphome.const import (
    CONF_FREQUENCY,
    PLATFORM_ESP32,
    PLATFORM_ESP8266,
)
DEPENDENCIES = ["cc1101", "remote_transmitter"]
AUTO_LOAD = ["cover"]

somfy_ns = cg.esphome_ns.namespace("somfy")
SomfyCover = somfy_ns.class_("SomfyCover", cover.Cover, cg.Component)

CONF_SOMFY_REMOTE_ADDRESS = "remote_address"
CONF_SOMFY_RF_FREQ = "rf_read_freq"
CONF_SOMFY_REPEAT = "repeat"
CONF_SOMFY_CC1101 = "cc1101"
CONF_REMOTE_TRANSMITTER = "remote_transmitter"

CONFIG_SCHEMA = cv.All(
  cover.cover_schema(SomfyCover).extend(
    {
        cv.Required(CONF_SOMFY_REMOTE_ADDRESS): cv.int_range(min=0, max=0xFFFFFFFF),
        cv.Optional(CONF_SOMFY_REPEAT, default=4): cv.int_range(min=1, max=16),
        cv.Optional(CONF_SOMFY_RF_FREQ, default="433.92MHz"): cv.All(
          cv.frequency, cv.float_range(min=300.0e6, max=928.0e6)
        ),
        cv.Optional(CONF_FREQUENCY, default="433.42MHz"): cv.All(
          cv.frequency, cv.float_range(min=300.0e6, max=928.0e6)
        ),
        cv.Required(CONF_SOMFY_CC1101): cv.use_id(cc1101.CC1101Component),
        cv.Required(CONF_REMOTE_TRANSMITTER): cv.use_id(remote_transmitter.RemoteTransmitterComponent),
    }
  ).extend(cv.COMPONENT_SCHEMA),
  cv.only_on([PLATFORM_ESP32, PLATFORM_ESP8266])
)


async def to_code(config):
    var = await cover.new_cover(config)
    await cg.register_component(var, config)

    cc1101 = await cg.get_variable(config[CONF_SOMFY_CC1101])
    cg.add(var.set_cc1101(cc1101))
    remote_transmitter = await cg.get_variable(config[CONF_REMOTE_TRANSMITTER])
    cg.add(var.set_remote_transmitter(remote_transmitter))


    cg.add(var.set_remote_address(config[CONF_SOMFY_REMOTE_ADDRESS]))
    cg.add(var.set_rf_freq(config[CONF_SOMFY_RF_FREQ]))
    cg.add(var.set_somfy_freq(config[CONF_FREQUENCY]))
    cg.add(var.set_repeat(config[CONF_SOMFY_REPEAT]))
