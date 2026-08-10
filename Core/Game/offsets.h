#pragma once
#include "thread"
#include <iostream>
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <algorithm>

// Updated to build 24614784 — 2026-08-09
// Source: cheatoffsets.com/g/rust

namespace O
{
    uintptr_t dwMainCamera = 0x107948D0; // MainCamera
    uintptr_t dwBaseNetworkable = 0x1074E028; // BaseNetworkable
    uintptr_t ghandle = 0x10C71E20; // Il2CppGetHandle
    uintptr_t Graphics = 0xBFF8B30;  // ConvarGraphics — NEEDS MANUAL UPDATE (not in dump)
    uintptr_t entitycount = 0x18; // PlayerList
	uintptr_t entityarray = 0x10; // EntitySize

    namespace BN // Basenetworkable
    {
        inline uintptr_t List1 = 0x20; // BaseNetworkable1
        inline uintptr_t List2 = 0x10; // BaseNetworkable2
		inline uintptr_t List3 = 0x10; // BaseNetworkable3
    }

    namespace Graphic
    {
        constexpr uint64_t fov = 0xC0; // updated
    }

    namespace BasePlayer
    {
        constexpr uint64_t playerFlags = 0x6D0; // updated build 24614784
        constexpr uint64_t playerTeam = 0x550; // CurrentTeam
        constexpr uint64_t Mounted = 0x5D8; // EntityRef
        constexpr uint64_t _displayName = 0x390; // updated
        constexpr uint64_t _Model = 0x1B8; // BaseEntity/BaseCombatEntity::Model
        constexpr uint64_t input = 0x3B8; // PlayerInput
        constexpr uint64_t PlayerModel = 0x340; // updated
        constexpr uint64_t PlayerEyes = 0x7A0; // updated
        constexpr uint64_t PlayerInventory = 0x4B0; // updated
        constexpr uint64_t PlayerMovement = 0x520; // BaseMovement
        constexpr uint64_t ModelState = 0x2D0; // obfuscated type

    }

    namespace BaseProjectile {
        inline int recoil = 0x400; // recoilproperties
        inline int automatic = 0x390; // upd
        inline int isBurstWeapon = 0x437; // upd
        inline int internalBurstFireRateScale = 0x440; //upd

        inline int viewModel = 0x250; // HeldEntity::viewModel
        inline int is_reloading = 0x3B8;
        inline int reloadDuration = 0x3D0; // reloadTime
        inline int primaryMagazine = 0x3D8; // updated
        inline int sightAimConeScale = 0x444;
        inline int hipAimConeScale = 0x414; // hipAimCone

        namespace viewModels {
            inline int baseviewModel = 0x28;
            namespace baseViewModel {
                inline int Animator = 0xD0;

                inline int ViewmodelBob = 0xC0;
                inline int viewmodelSway = 0x334;
                inline int viewmodelLower = 0xD8;
                inline int irongsights = 0x90;

                namespace ViewmodelLower {
                    inline int lowerOnSprint = 0x20;
                    inline int lowerWhenCantAttack = 0x21;
                    inline int shouldLower = 0x28;
                }
                namespace ViewmodelSway {
                    inline int positionalSwaySpeed = 0x338;
                    inline int rotationSwayAmount = 0x2C;
                }
                namespace viewmodelBob {
                    inline int bobAmountRun = 0x28;
                    inline int bobAmountWalk = 0x28;
                    inline int bobSpeedRun = 0x24;
                    inline int bobSpeedWalk = 0x20;
                }
            }
        }
    }

    namespace BaseProjectileExploits
    {
        constexpr uintptr_t automatic = 0x390; // updated
        constexpr uintptr_t FlintStrikeWeapon = 0x4A8; // successFraction
        constexpr uintptr_t DidShitTick = 0x400; // encrypted value — VERIFY
    }


    namespace RecoilProperties {
        constexpr uintptr_t new_recoil = 0x80;
        constexpr uintptr_t recoilYawMin = 0x18;
        constexpr uintptr_t recoilYawMax = 0x1C;
        constexpr uintptr_t recoilPitchMin = 0x20;
        constexpr uintptr_t recoilPitchMax = 0x24;
    }

    namespace PlayerInventory
    {
        constexpr uint64_t ClActiveItem = 0x580; // updated
        constexpr uint64_t Belt = 0x78; // updated
        constexpr uint64_t Wear = 0x38; // updated
        constexpr uint64_t ItemDisplayName = 0x40; // updated
        constexpr uint64_t ShortName = 0x28; // unchanged
        constexpr uint64_t ItemDisplayEnglish = 0x38;
        constexpr uint64_t ItemId = 0x88; // item::uid
        constexpr uint64_t ItemList = 0x38; // item_container::item_list

        constexpr uint64_t ItemDefinition = 0xD0; // item::definition
    }

    namespace PlayerModel
    {
        constexpr uintptr_t MaterialList = 0x148; // not updated
        constexpr uintptr_t SkinnedMultiMesh = 0x380; // updated
        constexpr uintptr_t RendererList = 0x40; // unchanged
        constexpr uint64_t new_velocity = 0x31C; // updated
    }

    namespace CameraManager
    {
        inline uintptr_t CameraObject = 0x38; // updated
    }

    namespace Recoil
    {
        constexpr uintptr_t RecoilProperties = 0x400; // updated
        constexpr uintptr_t NewRecoilOverride = 0x80; // unchanged
    }
    namespace Eyes
    {
        constexpr uintptr_t eye_offset = 0x400; // not in dump — keep as-is
        constexpr uintptr_t eye_rotation = 0x6C;
        constexpr uintptr_t body_rotation = 0x50;
        constexpr uintptr_t view_offset = 0x40;
    }

    namespace Sky
    {
        inline uintptr_t TOD_Sky = 0xBD23DD8; // NEEDS MANUAL UPDATE (not in dump)
    }

    namespace Model
    {
        inline uintptr_t boneTransforms = 0x50;
    }
    namespace Projectile
    {
        inline uintptr_t HeldItem = 0x10; // updated
        inline uintptr_t PrimaryMagazine = 0x3D8; // updated
    }
}
