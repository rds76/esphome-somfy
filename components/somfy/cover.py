import esphome.config_validation as cv
import esphome.codegen as cg
from esphome import automation
from esphome.components import cover, remote_transmitter, button
from esphome.const import (
    PLATFORM_ESP32,
    PLATFORM_ESP8266,
)
DEPENDENCIES = ["remote_transmitter"]
AUTO_LOAD = ["cover", "button"]

somfy_ns = cg.esphome_ns.namespace("somfy")
SomfyCover = somfy_ns.class_("SomfyCover", cover.Cover, cg.Component)

CONF_SOMFY_REMOTE_ADDRESS = "remote_address"
CONF_SOMFY_REPEAT = "repeat"
CONF_REMOTE_TRANSMITTER = "remote_transmitter"
CONF_PROG_BUTTON = "prog_button"
CONF_ON_TRANSMIT = "on_transmit"
CONF_ON_COMPLETE = "on_complete"

CONFIG_SCHEMA = cv.All(
  cover.cover_schema(SomfyCover).extend(
    {
        cv.Required(CONF_SOMFY_REMOTE_ADDRESS): cv.int_range(min=0, max=0xFFFFFFFF),
        cv.Optional(CONF_SOMFY_REPEAT, default=4): cv.int_range(min=1, max=16),
        cv.Required(CONF_REMOTE_TRANSMITTER): cv.use_id(remote_transmitter.RemoteTransmitterComponent),
        cv.Optional(CONF_PROG_BUTTON): cv.use_id(button.Button),
        cv.Optional(CONF_ON_TRANSMIT): automation.validate_automation(single=True),
        cv.Optional(CONF_ON_COMPLETE): automation.validate_automation(single=True),
    }
  ).extend(cv.COMPONENT_SCHEMA),
  cv.require_esphome_version(2026, 3, 0),
  cv.only_on([PLATFORM_ESP32, PLATFORM_ESP8266])
)


async def to_code(config):
    var = await cover.new_cover(config)
    await cg.register_component(var, config)

    remote_transmitter = await cg.get_variable(config[CONF_REMOTE_TRANSMITTER])
    cg.add(var.set_remote_transmitter(remote_transmitter))

    if on_transmit_config := config.get(CONF_ON_TRANSMIT):
        await automation.build_automation(
            var.get_transmit_trigger(), [], on_transmit_config
        )
    if on_complete_config := config.get(CONF_ON_COMPLETE):
        await automation.build_automation(
            var.get_complete_trigger(), [], on_complete_config
        )

    if CONF_PROG_BUTTON in config:
      btn = await cg.get_variable(config[CONF_PROG_BUTTON])
      cg.add(var.set_prog_button(btn))

    cg.add(var.set_remote_address(config[CONF_SOMFY_REMOTE_ADDRESS]))
    cg.add(var.set_repeat(config[CONF_SOMFY_REPEAT]))
