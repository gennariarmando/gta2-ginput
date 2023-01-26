#pragma once
#define PAD_API __declspec(dllexport)

#define MAX_PADS 2
#include <Xinput.h>

class PAD_API CControllerState {
public:
	short LeftStickX;
	short LeftStickY;
	short RightStickX;
	short RightStickY;
	short LeftShoulder1;
	short LeftShoulder2;
	short RightShoulder1;
	short RightShoulder2;
	short DPadUp;
	short DPadDown;
	short DPadLeft;
	short DPadRight;
	short Start;
	short Select;
	short Square;
	short Triangle;
	short Cross;
	short Circle;
	short LeftShock;
	short RightShock;

public:
	void Clear();
	bool CheckForInput();
};

class PAD_API CPad {
public:
	CControllerState NewState;
	CControllerState OldState;
	CControllerState TempState;

	int Index;
	bool DisableControls;
	char Mode;
	bool PadConnected;
	short ShakeDur;
	char ShakeFreq;

public:
	static CPad* GetPad(char pad);

	CPad();
	~CPad();

	void Update();
	void Clear();
	bool IsConnected();
	void StartShake(short dur, char freq);
	void StopShaking();

public:
	bool GetForward();
	bool GetBackward();
	bool GetLeft();
	bool GetRight();
	bool GetFire();
	bool GetEnterExitVehicle();
	bool GetJump();
	bool GetCycleWeaponLeft();
	bool GetCycleWeaponRight();
	bool GetSpecial();
};


extern CPad* Pads[MAX_PADS];
extern XINPUT_STATE XInputState[MAX_PADS];
extern XINPUT_VIBRATION XInputVibration[MAX_PADS];
