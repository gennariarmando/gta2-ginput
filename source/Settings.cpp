#include "plugin.h"
#include "Settings.h"

Settings settings;

void Settings::Read() {
    plugin::config_file config(PLUGIN_PATH("GInputII.ini"));

    // GInput
    VibrateOnRecon = config["VibrateOnRecon"].asBool(true);
    Vibration = config["Vibration"].asBool(true);

    // Pad
    ControlSetP1 = config["ControlSetP1"].asInt(0);
    ControlSetP2 = config["ControlSetP2"].asInt(0);
}
