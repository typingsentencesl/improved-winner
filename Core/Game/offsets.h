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
    uintptr_t Graphics = 0xBFF8B30;  // ConvarGraphics
    uintptr_t entitycount = 0x18; // PlayerList
    uintptr_t entityarray = 0x10; // EntitySize

    namespace BN // Basenetworkable
    {
        inline uintptr_t List1 = 0x20; // BaseNetworkable1 / entities
        inline uintptr_t List2 = 0x10; // BaseNetworkable2 / parent_static_fields
        inline uintptr_t List3 = 0x10; // BaseNetworkable3
    }

    namespace Graphic
    {
        constexpr uint64_t fov = 0xC0;
    }

    namespace BasePlayer
    {
        constexpr uint64_t playerFlags = 0x6D0; // updated build 24614784
        constexpr uint64_t playerTeam = 0x550; // current_team
        constexpr uint64_t Mounted = 0x5D8; // mounted
        constexpr uint64_t _displayName = 0x390; // display_name
        constexpr uint64_t _Model = 0x1B8; // model
        constexpr uint64_t input = 0x3B8; // player_input
        constexpr uint64_t PlayerModel = 0x340; // player_model
        constexpr uint64_t PlayerEyes = 0x7A0; // player_eyes
        constexpr uint64_t PlayerInventory = 0x4B0; // player_inventory
        constexpr uint64_t PlayerMovement = 0x520; // base_movement
        constexpr uint64_t ModelState = 0x2D0; // model_state
        constexpr uint64_t HeldEntity = 0x310; // held_entity
    }

    namespace BaseProjectile {
        inline int recoil = 0x400; // recoil
        inline int automatic = 0x390; // automatic
        inline int isBurstWeapon = 0x437; // isBurstWeapon
        inline int internalBurstFireRateScale = 0x440; // internalBurstFireRateScale

        inline int viewModel = 0x250; // held_entity::viewModel
        inline int is_reloading = 0x3B8;
        inline int reloadDuration = 0x3D0; // reloadTime
        inline int primaryMagazine = 0x3D8; // primaryMagazine
        inline int sightAimConeScale = 0x444; // sightAimConeScale
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
        constexpr uintptr_t automatic = 0x390;
        constexpr uintptr_t FlintStrikeWeapon = 0x4A8; // successFraction
        constexpr uintptr_t DidShitTick = 0x400;
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
        constexpr uint64_t ClActiveItem = 0x580; // cl_active_item
        constexpr uint64_t Belt = 0x78; // belt
        constexpr uint64_t Wear = 0x38; // wear
        constexpr uint64_t Main = 0x58; // main
        constexpr uint64_t ItemDisplayName = 0x40; // displayName
        constexpr uint64_t ShortName = 0x28; // shortname
        constexpr uint64_t ItemDisplayEnglish = 0x38;
        constexpr uint64_t ItemId = 0x88; // uid
        constexpr uint64_t ItemList = 0x38; // item_list
        constexpr uint64_t ItemDefinition = 0xD0; // definition
    }

    namespace PlayerModel
    {
        constexpr uintptr_t MaterialList = 0x148;
        constexpr uintptr_t SkinnedMultiMesh = 0x380; // skinnedMultiMesh
        constexpr uintptr_t RendererList = 0x40; // renderers
        constexpr uint64_t new_velocity = 0x31C; // velocity
        constexpr uint64_t position = 0x2F8; // position
    }

    namespace CameraManager
    {
        inline uintptr_t CameraObject = 0x38; // camera_object
        inline uintptr_t ViewMatrix = 0x2FC; // view_matrix
        inline uintptr_t Position = 0x444; // position
    }

    namespace Recoil
    {
        constexpr uintptr_t RecoilProperties = 0x400;
        constexpr uintptr_t NewRecoilOverride = 0x80;
    }

    namespace Eyes
    {
        constexpr uintptr_t eye_offset = 0x400;
        constexpr uintptr_t eye_rotation = 0x6C;
        constexpr uintptr_t body_rotation = 0x50;
        constexpr uintptr_t view_offset = 0x40;
        constexpr uintptr_t world_position = 0x60;
    }

    namespace Sky
    {
        inline uintptr_t TOD_Sky = 0xBD23DD8;
    }

    namespace Model
    {
        inline uintptr_t boneTransforms = 0x50; // boneTransforms
    }

    namespace Projectile
    {
        inline uintptr_t HeldItem = 0x10;
        inline uintptr_t PrimaryMagazine = 0x3D8;
    }
}

