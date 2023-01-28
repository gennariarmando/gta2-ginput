#include "plugin.h"
#include "Settings.h"

Settings settings;

void Settings::Read() {
    plugin::config_file config(PLUGIN_PATH("GInputII.ini"));

    // GInput
    Vibration = config["Vibration"].asBool();
    MapPadOneToPadTwo = config["MapPadOneToPadTwo"].asBool();

    // Pad1
    ControlSetP1 = config["ControlSetP1"].asInt();
    LeftStickDeadZoneP1 = config["LeftStickDeadZoneP1"].asInt() / 100.0f;

    // Pad2
    ControlSetP2 = config["ControlSetP2"].asInt();
    LeftStickDeadZoneP2 = config["LeftStickDeadZoneP2"].asInt() / 100.0f;
}
