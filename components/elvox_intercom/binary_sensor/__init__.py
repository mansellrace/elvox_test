import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ID, CONF_ICON
from .. import elvox_intercom_ns, ElvoxIntercom, CONF_ELVOX_ID

ElvoxIntercomBinarySensor = elvox_intercom_ns.class_(
    "ElvoxIntercomBinarySensor", binary_sensor.BinarySensor, cg.Component
)

CONF_HEX = "hex"
CONF_NAME = "name"
CONF_AUTO_OFF = "auto_off"

DEPENDENCIES = ["elvox_intercom"]

CONFIG_SCHEMA = cv.All(
    binary_sensor.binary_sensor_schema(ElvoxIntercomBinarySensor).extend(
        {
            cv.GenerateID(): cv.declare_id(ElvoxIntercomBinarySensor),
            cv.GenerateID(CONF_ELVOX_ID): cv.use_id(ElvoxIntercom),
            cv.Required(CONF_HEX): cv.string,
            cv.Optional(CONF_ICON, default="mdi:doorbell"): cv.icon,
            cv.Optional(CONF_NAME, default="Incoming call"): cv.string,
            cv.Optional(CONF_AUTO_OFF, default="30s"): cv.positive_time_period_seconds
        }
    ),
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await binary_sensor.register_binary_sensor(var, config)
    cg.add(var.set_hex(config[CONF_HEX]))
    cg.add(var.set_auto_off(config[CONF_AUTO_OFF]))
    elvox_intercom = await cg.get_variable(config[CONF_ELVOX_ID])
    cg.add(elvox_intercom.register_listener(var))