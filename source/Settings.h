#pragma once

class Settings {
public:
    bool VibrateOnRecon;
    bool Vibration;

    int ControlSetP1;
    int ControlSetP2;

public:
    void Read();
};

extern Settings settings;
