#pragma once

class Settings {
public:
    bool Vibration;
    bool MapPadOneToPadTwo;

    int ControlSetP1;
    int ControlSetP2;

    float LeftStickDeadZoneP1;
    float LeftStickDeadZoneP2;


public:
    void Read();
};

extern Settings settings;
