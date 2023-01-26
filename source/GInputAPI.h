/*
 *
 * GTA2 - GInput API ver 1.0 - 26/01/2023
 * Usage: Add this file to your project, call CPad::GetPad(x).
 *
 */
#pragma once
#include "ModuleList.hpp"

struct ApiCALL {
	static void* GetFunctionByName(const char* name) {
		const HMODULE h = ModuleList().Get(L"GInputII");
		if (h) {
			auto a = (void* (*)())GetProcAddress(h, name);

			if (a) {
				return a;
			}
		}
		return NULL;
	}

	template <typename... Args>
	static void Call(const char* name, Args... args) {
		void* f = GetFunctionByName(name);
		if (f)
			reinterpret_cast<void(__cdecl*)(Args...)>(f)(args...);
	}

	template <typename Ret, typename... Args>
	static Ret CallAndReturn(const char* name, Args... args) {
		void* f = GetFunctionByName(name);

		if (f)
			return reinterpret_cast<Ret(__cdecl*)(Args...)>(f)(args...);

		return NULL;
	}

	template <typename... Args>
	static void CallMethod(const char* name, Args... args) {
		void* f = GetFunctionByName(name);
		if (f)
			reinterpret_cast<void(__thiscall*)(Args...)>(f)(args...);
	}

	template <typename Ret, typename... Args>
	static Ret CallMethodAndReturn(const char* name, Args... args) {
		void* f = GetFunctionByName(name);

		if (f)
			return reinterpret_cast<Ret(__thiscall*)(Args...)>(f)(args...);

		return NULL;
	}
};

class CControllerState {
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
	void Clear() {
		ApiCALL::Call(__FUNCTION__, this);
	}

	bool CheckForInput() {
		return ApiCALL::CallMethodAndReturn<bool>(__FUNCTION__, this);
	}
};

class CPad {
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
	static CPad* GetPad(char pad) {
		return ApiCALL::CallAndReturn<CPad*>(__FUNCTION__, pad);
	}
	
	CPad() {
		ApiCALL::CallMethod(__FUNCTION__, this);
	}
	~CPad(){}

	void Update() {
		ApiCALL::CallMethod(__FUNCTION__, this);
	}
	void Clear() {
		ApiCALL::CallMethod(__FUNCTION__, this);
	}
	bool IsConnected() {
		return ApiCALL::CallMethodAndReturn<bool>(__FUNCTION__, this);
	}
	void StartShake(short dur, char freq) {
		ApiCALL::CallMethod(__FUNCTION__, this);
	}
	void StopShaking() {
		ApiCALL::CallMethod(__FUNCTION__, this);
	}

public:
	bool GetForward() {
		return ApiCALL::CallMethodAndReturn<bool>(__FUNCTION__, this);
	}
	bool GetBackward() {
		return ApiCALL::CallMethodAndReturn<bool>(__FUNCTION__, this);
	}
	bool GetLeft() {
		return ApiCALL::CallMethodAndReturn<bool>(__FUNCTION__, this);
	}
	bool GetRight() {
		return ApiCALL::CallMethodAndReturn<bool>(__FUNCTION__, this);
	}
	bool GetFire() {
		return ApiCALL::CallMethodAndReturn<bool>(__FUNCTION__, this);
	}
	bool GetEnterExitVehicle() {
		return ApiCALL::CallMethodAndReturn<bool>(__FUNCTION__, this);
	}
	bool GetJump() {
		return ApiCALL::CallMethodAndReturn<bool>(__FUNCTION__, this);
	}
	bool GetCycleWeaponLeft() {
		return ApiCALL::CallMethodAndReturn<bool>(__FUNCTION__, this);
	}
	bool GetCycleWeaponRight() {
		return ApiCALL::CallMethodAndReturn<bool>(__FUNCTION__, this);
	}
	bool GetSpecial() {
		return ApiCALL::CallMethodAndReturn<bool>(__FUNCTION__, this);
	}
};
