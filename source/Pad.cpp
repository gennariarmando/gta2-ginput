#include "plugin.h"
#include "Pad.h"
//#include "PadAPI.h"

#pragma comment(lib, "Xinput9_1_0.lib")

CPad* Pads[MAX_PADS];
XINPUT_STATE XInputState[MAX_PADS];
XINPUT_VIBRATION XInputVibration[MAX_PADS];

int PadsCount = 0;

void CControllerState::Clear() {
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)

	LeftStickX = 0;
	LeftStickY = 0;
	RightStickX = 0;
	RightStickY = 0;
	LeftShoulder1 = 0;
	LeftShoulder2 = 0;
	RightShoulder1 = 0;
	RightShoulder2 = 0;
	DPadUp = 0;
	DPadDown = 0;
	DPadLeft = 0;
	DPadRight = 0;
	Start = 0;
	Select = 0;
	Square = 0;
	Triangle = 0;
	Cross = 0;
	Circle = 0;
	LeftShock = 0;
	RightShock = 0;
}

bool CControllerState::CheckForInput() {
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)

	return !!RightStickX || !!RightStickY || !!LeftStickX || !!LeftStickY
		|| !!DPadUp || !!DPadDown || !!DPadLeft || !!DPadRight
		|| !!Triangle || !!Cross || !!Circle || !!Square
		|| !!Start || !!Select
		|| !!LeftShoulder1 || !!LeftShoulder2 || !!RightShoulder1 || !!RightShoulder2
		|| !!LeftShock || !!RightShock;
}

CPad* CPad::GetPad(char pad) {
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)
	return Pads[pad];
}

CPad::CPad() {
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)

	Clear();

	Index = PadsCount;
	PadsCount++;

	DisableControls = false;
	Mode = 0;
	PadConnected = false;
	ShakeDur = 0;
	ShakeFreq = 1;
}

CPad::~CPad() {
	PadsCount--;
}

void CPad::Update() {
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)

	XINPUT_STATE& state = XInputState[Index];

	ZeroMemory(&state, sizeof(XINPUT_STATE));
	if (XInputGetState(0, &state) == ERROR_SUCCESS) {
		if (!PadConnected) {
			StartShake(250, 250);
			PadConnected = true;
		}

		OldState = NewState;
		NewState = TempState;

		float normLX = fmaxf(-1.0f, state.Gamepad.sThumbLX / 32767.0f);
		float normLY = fmaxf(-1.0f, state.Gamepad.sThumbLY / 32767.0f);
		float normRX = fmaxf(-1.0f, state.Gamepad.sThumbRX / 32767.0f);
		float normRY = fmaxf(-1.0f, state.Gamepad.sThumbRY / 32767.0f);

		float deadzoneX = 0.02f;
		float deadzoneY = 0.02f;
		float leftStickX = (abs(normLX) < deadzoneX ? 0 : normLX);
		float leftStickY = (abs(normLY) < deadzoneY ? 0 : normLY);
		float rightStickX = (abs(normRX) < deadzoneX ? 0 : normRX);
		float rightStickY = (abs(normRY) < deadzoneY ? 0 : normRY);

		TempState.LeftStickX = leftStickX;
		TempState.LeftStickY = leftStickY;
		TempState.RightStickX = rightStickX;
		TempState.RightStickY = rightStickY;

		float val = state.Gamepad.bLeftTrigger;
		TempState.LeftShoulder2 = static_cast<short>(val > XINPUT_GAMEPAD_TRIGGER_THRESHOLD ? (val - XINPUT_GAMEPAD_TRIGGER_THRESHOLD) * (255.0f / (255.0f - XINPUT_GAMEPAD_TRIGGER_THRESHOLD)) : 0);

		val = state.Gamepad.bRightTrigger;
		TempState.RightShoulder2 = static_cast<short>(val > XINPUT_GAMEPAD_TRIGGER_THRESHOLD ? (val - XINPUT_GAMEPAD_TRIGGER_THRESHOLD) * (255.0f / (255.0f - XINPUT_GAMEPAD_TRIGGER_THRESHOLD)) : 0);

		TempState.LeftShoulder1 = (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) ? 255 : 0;
		TempState.RightShoulder1 = (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ? 255 : 0;

		TempState.DPadUp = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) ? 255 : 0;
		TempState.DPadDown = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) ? 255 : 0;
		TempState.DPadLeft = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) ? 255 : 0;
		TempState.DPadRight = (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) ? 255 : 0;

		TempState.Start = (state.Gamepad.wButtons & XINPUT_GAMEPAD_START) ? 255 : 0;
		TempState.Select = (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) ? 255 : 0;

		TempState.Square = (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) ? 255 : 0;
		TempState.Triangle = (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) ? 255 : 0;
		TempState.Cross = (state.Gamepad.wButtons & XINPUT_GAMEPAD_A) ? 255 : 0;
		TempState.Circle = (state.Gamepad.wButtons & XINPUT_GAMEPAD_B) ? 255 : 0;

		TempState.LeftShock = (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) ? 255 : 0;
		TempState.RightShock = (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) ? 255 : 0;

		unsigned short speed = 0;
		if (ShakeDur > 0) {
			speed = ShakeFreq * 257;
			ShakeDur -= 1.0f / 50.0f * 1000.0f;
			if (ShakeDur <= 0) {
				ShakeDur = 0;
				speed = 0;
			}

			XINPUT_VIBRATION& v = XInputVibration[Index];
			v.wLeftMotorSpeed = speed;
			v.wRightMotorSpeed = 0;
			XInputSetState(Index, &v);
		}
	}
	else {
		PadConnected = false;
		Clear();
	}
}

void CPad::StartShake(short dur, char freq) {
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)

	if (freq == 0) {
		ShakeDur = 0;
		ShakeFreq = 0;
		return;
	}

	if (dur > ShakeDur) {
		ShakeDur = dur;
		ShakeFreq = freq;
	}
}

void CPad::StopShaking() {
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)

	ShakeFreq = 0;
	ShakeDur = 0;

	XINPUT_VIBRATION& v = XInputVibration[Index];
	v.wLeftMotorSpeed = 0;
	v.wRightMotorSpeed = 0;
	XInputSetState(Index, &v);
}

void CPad::Clear() {
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)

	OldState.Clear();
	NewState.Clear();
	TempState.Clear();
}

bool CPad::IsConnected() {
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)

	return PadConnected;
}

bool CPad::GetForward() {
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)

	if (DisableControls)
		return false;

	switch (Mode) {
	case 0:
	case 1:
	case 4:
		return NewState.Cross;
	case 2:
		return NewState.Triangle;
	case 3:
		return NewState.LeftShoulder1;
	}

	return false;
}

bool CPad::GetBackward() {
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)

	if (DisableControls)
		return false;

	switch (Mode) {
	case 0:
		return NewState.Square;
	case 1:
		return NewState.Triangle;
	case 2:
		return NewState.Cross;
	case 3:
	case 4:
		return NewState.LeftShoulder2;
	}

	return false;
}

bool CPad::GetLeft() {
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)

	if (DisableControls)
		return false;

	switch (Mode) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		return NewState.DPadLeft || NewState.LeftStickX < 0;
	}

	return false;
}

bool CPad::GetRight() {
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)

	if (DisableControls)
		return false;

	switch (Mode) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		return NewState.DPadRight || NewState.LeftStickX > 0;
	}

	return false;
}

bool CPad::GetFire() {
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)

	if (DisableControls)
		return false;

	switch (Mode) {
	case 0:
		return NewState.RightShoulder2;
	case 1:
		return NewState.Circle;
	case 2:
		return NewState.RightShoulder1;
	case 3:
	case 4:
		return NewState.Triangle;
	}

	return false;
}

bool CPad::GetEnterExitVehicle() {
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)

	if (DisableControls)
		return false;

	switch (Mode) {
	case 0:
		return NewState.Triangle && !OldState.Triangle;
	case 1:
		return NewState.Square && !OldState.Square;
	case 2:
		return NewState.RightShoulder2 && !OldState.RightShoulder2;
	case 3:
		return NewState.Cross && !OldState.Cross;
	case 4:
		return NewState.Circle && !OldState.Circle;
	}

	return false;
}

bool CPad::GetJump() {
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)

	if (DisableControls)
		return false;

	switch (Mode) {
	case 0:
	case 1:
		return NewState.RightShoulder1 && !OldState.RightShoulder1;
	case 2:
	case 3:
		return NewState.Circle && !OldState.Circle;
	case 4:
		return NewState.LeftShoulder1 && !OldState.LeftShoulder1;
	}

	return false;
}

bool CPad::GetCycleWeaponLeft() {
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)

	if (DisableControls)
		return false;

	switch (Mode) {
	case 0:
	case 1:
	case 2:
		return NewState.LeftShoulder2 && !OldState.LeftShoulder2;
	case 3:
	case 4:
		return NewState.RightShoulder2 && !OldState.RightShoulder2;
	}

	return false;
}

bool CPad::GetCycleWeaponRight() {
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)

	if (DisableControls)
		return false;

	switch (Mode) {
	case 0:
	case 1:
	case 2:
		return NewState.LeftShoulder1 && !OldState.LeftShoulder1;
	case 3:
	case 4:
		return NewState.RightShoulder1 && !OldState.RightShoulder1;
	}

	return false;
}

bool CPad::GetSpecial() {
#pragma comment(linker, "/EXPORT:" __FUNCTION__"=" __FUNCDNAME__)

	if (DisableControls)
		return false;

	switch (Mode) {
	case 0:
		return NewState.Circle && !OldState.Circle;
	case 1:
		return NewState.RightShoulder2 && OldState.RightShoulder2;
	case 2:
	case 3:
	case 4:
		return NewState.Square && !OldState.Square;
	}

	return false;
}
