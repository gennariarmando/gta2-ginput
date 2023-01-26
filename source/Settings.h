#pragma once

class Settings {
public:
    bool VibrateOnRecon;

    int ControlSetP1;
    int ControlSetP2;


public:
    void Read();
};

extern Settings settings;
