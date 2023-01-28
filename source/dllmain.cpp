#include "plugin.h"
#include "Pad.h"
#include "Settings.h"

#include "CMenuManager.h"
#include "CPlayerPed.h"
#include "CPed.h"
#include "CGame.h"
#include "CText.h"
#include "CChar.h"
#include "CGame.h"
#include "common.h"
#include "CReplay.h"
#include "CFont.h"
#include "CCamera.h"

using namespace plugin;


class GTA2GInput {
public:
    static void ProcessVibration(int index, CPad* pad, CPed* ped) {
        if (!ped || ped->m_nHealth <= 0)
            return;

        CWeapon* weapon = ped->m_pSelectedWeapon;
        CCar* car = ped->m_pCurrentCar;

        if (weapon) {
            static int previousAmmo[MAX_PADS] = { -1, -1 };

            if (weapon->m_nTimer && weapon->m_nAmmo != previousAmmo[index]) {
                if (previousAmmo[index] != -1)
                    pad->StartShake(100, 120);
                previousAmmo[index] = weapon->m_nAmmo;
            }
        }

        static short previousHealth[MAX_PADS] = { -1, -1 };
        if (previousHealth[index] != ped->m_nHealth) {
            if (previousHealth[index] != -1)
                pad->StartShake(100, 80);
            previousHealth[index] = ped->m_nHealth;
        }

        if (car) {
            CCarPhysics* physics = car->m_pPhysics;
            if ((physics->m_vVelocity.FromInt16().Magnitude() > 0.01f && physics->m_nTileSurfaceType == TILE_SURFACE_TYPE_GRASS)) {
                pad->StartShake(50, 80);
            }

            static int previousDamage[MAX_PADS] = { -1, -1 };
            if (car->m_nDamage != previousDamage[index]) {
                if (previousDamage[index] != -1)
                    pad->StartShake(50, 80);
                previousDamage[index] = car->m_nDamage;
            }
        }
    }

    static void ProcessPlayerControls(CPlayerPed* _this) {
        if (gReplay->IsPlayingBack() || GetGame()->GetIsUserPaused()) {
            return;
        }

        CPed* ped = _this->m_pPed;

        if (!ped)
            return;

        CCar* car = ped->m_pCurrentCar;
        int index = _this->m_nIndex;

        if (network_game && index > 0)
            return;

        if (index > MAX_PADS - 1)
            return;

        if (settings.MapPadOneToPadTwo && index == 0)
            return;
        else if (settings.MapPadOneToPadTwo && index == 1)
            index = 0;

        CPad* pad = CPad::GetPad(index);

        // Player
        _this->m_bButtonForward |= pad->GetForward();
        _this->m_bOldButtonForward |= pad->GetForward();

        _this->m_bButtonBackward |= pad->GetBackward();
        _this->m_bOldButtonBackward |= pad->GetBackward();

        _this->m_bButtonLeft |= pad->GetLeft();
        _this->m_bButtonRight |= pad->GetRight();

        _this->m_bButtonAttack |= pad->GetFire();
        _this->m_bOldButtonAttack |= !pad->GetFire();

        _this->m_bButtonEnterExit |= pad->GetEnterExitVehicle();
        _this->m_bOldButtonEnterExit |= pad->GetEnterExitVehicle();

        bool vehicle = ped && car;
        _this->m_bButtonHandbrakeJump |= vehicle ? pad->GetHandbrake() : pad->GetJump();
        _this->m_bOldButtonHandbrakeJump |= vehicle ? pad->GetHandbrake() : pad->GetJump();

        _this->m_bButtonPrevWeapon |= pad->GetCycleWeaponLeft();
        _this->m_bOldButtonPrevWeapon |= pad->GetCycleWeaponLeft();

        _this->m_bButtonNextWeapon |= pad->GetCycleWeaponRight();
        _this->m_bOldButtonNextWeapon |= pad->GetCycleWeaponRight();

        _this->m_bButtonSpecial |= vehicle ? pad->GetHorn() : pad->GetSpecial();
        _this->m_bOldButtonSpecial |= vehicle ? pad->GetHorn() : pad->GetSpecial();

        // Vibration
        if (settings.Vibration)
            ProcessVibration(_this->m_nIndex, pad, ped);
    }

    static void ProcessPad(CGame* game) {
        for (auto& it : Pads)
            it->Update();

        int* key = &game->m_pCurrentPlayer->m_nKeyboardKey;
        CPad::GetPad(0)->UpdateKeyboard(key, false);

        if (gReplay->IsPlayingBack()) {
            if (CPad::GetPad(0)->NewState.CheckForInput())
                GetGame()->SetState(0, 6);
            return;
        }

        CPlayerPed* playa = game->m_pCurrentPlayer;
        CPad* pad = CPad::GetPad(0);
        if (pad->NewState.Start && !pad->OldState.Start) {
            playa->ProcessKeyPresses(DIK_F6);
            playa->m_bShowGameQuitText = false;
        }

        if (GetGame()->GetIsUserPaused()) {
            if (pad->NewState.Select && !pad->OldState.Select) {
                playa->ProcessKeyPresses(DIK_ESCAPE);
            }
        }
        else {
            if (pad->NewState.Select && !pad->OldState.Select) {
                playa->ProcessKeyPresses(DIK_F7);
            }
        }

        if (playa->m_bShowGameQuitText) {
            if (pad->NewState.Cross && !pad->OldState.Cross)
                playa->ProcessKeyPresses(DIK_RETURN);
        }

        pad->DisableControls = GetGame()->GetIsUserPaused();
    }

    static void ProcessMenuControls(CMenuManager* _this) {
        CPad* pad = CPad::GetPad(0);
        pad->Update();
        pad->UpdateKeyboard((int*)_this->m_nKeys, true);

        _this->OldKeyState.left |= pad->OldState.DPadLeft ? 1 : 0;
        _this->OldKeyState.right |= pad->OldState.DPadRight ? 1 : 0;

        _this->NewKeyState.left |= (pad->NewState.DPadLeft && !pad->OldState.DPadLeft) ? 1 : 0;
        _this->NewKeyState.right |= (pad->NewState.DPadRight && !pad->OldState.DPadRight) ? 1 : 0;
        _this->NewKeyState.up |= (pad->NewState.DPadUp && !pad->OldState.DPadUp) ? 1 : 0;
        _this->NewKeyState.down |= (pad->NewState.DPadDown && !pad->OldState.DPadDown) ? 1 : 0;
        _this->NewKeyState.enter |= (pad->NewState.Cross && !pad->OldState.Cross) ? 1 : 0;
        _this->NewKeyState.esc |= (pad->NewState.Triangle && !pad->OldState.Triangle) ? 1 : 0;
        _this->NewKeyState.del |= (pad->NewState.Square && !pad->OldState.Square) ? 1 : 0;

        if (pad->NewState.CheckForInput())
            _this->m_nKeys[0] = -1;
    }

    static void InitPad() {
        settings.Read();

        Pads[0] = new CPad();
        Pads[0]->Mode = settings.ControlSetP1;
        Pads[0]->StickDeadZone = settings.LeftStickDeadZoneP1;

        Pads[1] = new CPad();
        Pads[1]->Mode = settings.ControlSetP1;
        Pads[1]->StickDeadZone = settings.LeftStickDeadZoneP2;
    }

    static void ShutdownPad() {
        delete Pads[0];
        delete Pads[1];
    }

    GTA2GInput() {
        plugin::Events::initEngineEvent.before += []() {
            InitPad();
        };

        plugin::Events::shutdownEngineEvent.before += []() {
            ShutdownPad();
        };

        ThiscallEvent <AddressList<0x45A253, H_CALL, 0x45A415, H_CALL, 0x45348E, H_CALL>, PRIORITY_AFTER, ArgPickN<CMenuManager*, 0>, void(CMenuManager*)> onGetMenuKeyStates;
        onGetMenuKeyStates += [](CMenuManager* _this) {
            ProcessMenuControls(_this);
        };

        ThiscallEvent <AddressList<0x45C3B3, H_CALL>, PRIORITY_AFTER, ArgPickN<CGame*, 0>, void(CGame*)> onPadUpdate;
        onPadUpdate.before += [](CGame* game) {
            ProcessPad(game);
        };

        ThiscallEvent <AddressList<0x4A7683, H_CALL>, PRIORITY_AFTER, ArgPickN<CPlayerPed*, 0>, void(CPlayerPed*)> onGetPlayerKeyStates;
        onGetPlayerKeyStates += [](CPlayerPed* _this) {
            ProcessPlayerControls(_this);
        };
    }
} gta2GInput;
