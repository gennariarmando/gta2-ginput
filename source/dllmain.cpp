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

using namespace plugin;

class GTA2GInput {
public:
    static void ProcessVibration(CPed* ped) {
        if (!ped || ped->m_nHealth <= 0)
            return;

        CPad* pad = CPad::GetPad(0);

        CWeapon* weapon = ped->m_pSelectedWeapon;
        CCar* car = ped->m_pCurrentCar;

        if (weapon) {
            static int previousAmmo = 0;

            if (weapon->m_nTimer && weapon->m_nAmmo != previousAmmo) {
                pad->StartShake(100, 120);
                previousAmmo = weapon->m_nAmmo;
            }
        }

        static short previousHealth = ped->m_nHealth;
        if (previousHealth != ped->m_nHealth) {
            pad->StartShake(100, 80);
            previousHealth = ped->m_nHealth;
        }


        if (car) {
            CCarPhysics* physics = car->m_pPhysics;
            if ((physics->m_vVelocity.FromInt16().Magnitude() > 0.01f && physics->m_nTileSurfaceType == TILE_SURFACE_TYPE_GRASS)) {
                pad->StartShake(50, 80);
            }

            static int previousDamage = car->m_nDamage;
            if (car->m_nDamage != previousDamage) {
                pad->StartShake(50, 80);
                previousDamage = car->m_nDamage;
            }
        }
    }

    GTA2GInput() {
        plugin::Events::initEngineEvent.before += []() {
            settings.Read();

            Pads[0] = new CPad();
            Pads[0]->Mode = settings.ControlSetP1;

            Pads[1] = new CPad();
            Pads[1]->Mode = settings.ControlSetP1;
        };

        plugin::Events::shutdownEngineEvent += []() {
            delete Pads[0];
            delete Pads[1];
        };

        ThiscallEvent <AddressList<0x45A253, H_CALL, 0x45A415, H_CALL>, PRIORITY_AFTER, ArgPickN<CMenuManager*, 0>, void(CMenuManager*)> onGetMenuKeyStates;
        onGetMenuKeyStates += [](CMenuManager* _this) {
            CPad* pad = CPad::GetPad(0);
            pad->Update();


            _this->OldKeyState.left |= pad->OldState.DPadLeft ? 1 : 0;
            _this->OldKeyState.right |= pad->OldState.DPadRight ? 1 : 0;

            _this->NewKeyState.left |= (pad->NewState.DPadLeft && !pad->OldState.DPadLeft) ? 1 : 0;
            _this->NewKeyState.right |= (pad->NewState.DPadRight && !pad->OldState.DPadRight) ? 1 : 0;
            _this->NewKeyState.up |= (pad->NewState.DPadUp && !pad->OldState.DPadUp) ? 1 : 0;
            _this->NewKeyState.down |= (pad->NewState.DPadDown && !pad->OldState.DPadDown) ? 1 : 0;
            _this->NewKeyState.enter |= (pad->NewState.Cross && !pad->OldState.Cross) ? 1 : 0;
            _this->NewKeyState.esc |= (pad->NewState.Triangle && !pad->OldState.Triangle) ? 1 : 0;
            _this->NewKeyState.del |= (pad->NewState.Square && !pad->OldState.Square) ? 1 : 0;
        };

        ThiscallEvent <AddressList<0x4A7683, H_CALL>, PRIORITY_AFTER, ArgPickN<CPlayerPed*, 0>, void(CPlayerPed*)> onGetPlayerKeyStates;
        onGetPlayerKeyStates += [](CPlayerPed* _this) {
            if (_this == GetGame()->m_pCurrentPlayer) {
                CPed* ped = _this->m_pPed;
                CCar* car = ped->m_pCurrentCar;
                CPad* pad = CPad::GetPad(0);
                pad->Update();


                // Player
                bool b = _this->m_bButtonForward;
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

                // Menu
                if (pad->NewState.Start && !pad->OldState.Start) {
                    _this->ProcessKeyPresses(DIK_F6);
                    _this->m_bShowGameQuitText = false;
                }

                if (GetGame()->GetIsUserPaused()) {
                    if (pad->NewState.Select && !pad->OldState.Select) {
                        _this->ProcessKeyPresses(DIK_ESCAPE);
                    }
                }
                else {
                    if (pad->NewState.Select && !pad->OldState.Select) {
                        _this->ProcessKeyPresses(DIK_F7);
                    }
                }

                if (_this->m_bShowGameQuitText) {
                    if (pad->NewState.Cross && !pad->OldState.Cross)
                        _this->ProcessKeyPresses(DIK_RETURN);
                }

                pad->DisableControls = GetGame()->GetIsUserPaused();

                // Vibration
                if (settings.Vibration)
                    ProcessVibration(_this->m_pPed);
            }
        };
    }
} gta2GInput;
