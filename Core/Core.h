#define NOMINMAX  
#include <WinNls.h>
#include "corecrt_math.h"
#include <unordered_map>
#include <algorithm> 
#include <thread>
#include <chrono>
#include <map>
#include <string>
#include "math.h"

#define M_PI 3.14159265358979323846f
#define M_RADPI	57.295779513082f
#define M_PI_F ((float)(M_PI))
#define RAD2DEG(x) ((float)(x) * (float)(180.f / M_PI_F))

bool Valid(const uint64_t adress)
{
    if (adress <= 0x400000 || adress == 0xCCCCCCCCCCCCCCCC || reinterpret_cast<void*>(adress) == nullptr || adress >
        0x7FFFFFFFFFFFFFFF) {
        return false;
    }
    return true;
}


//void AimAtTarget(Vector2 targetScreenPos, float smoothFactor) {
//    POINT cursorPos;
//    GetCursorPos(&cursorPos);
//    int deltaX = static_cast<int>((targetScreenPos.x - Backend::width / 2) * smoothFactor);
//    int deltaY = static_cast<int>((targetScreenPos.y - Backend::height / 2) * smoothFactor);
//    move_mouse(deltaX, deltaY);
//}

struct Data {
    BasePlayer* LocalPlayer = nullptr;
};

extern std::unique_ptr<Data> data;
std::unique_ptr<Data> data = std::make_unique<Data>();

struct ESP {

    struct PlayerList {
        std::uint64_t Address()
        {
            return (std::uint64_t)(this);
        }

        BasePlayer* ptr;
        Vector3 position;
        BasePlayer::bounds_t bounds;
        std::string name;

    };

    struct ESPData {
        std::string gamebuff = "";
        BasePlayer* player;
        BaseNetworkable* entity;
        BaseNetworkable* entity_object;
    };


    PlayerList m_player;
    std::vector<PlayerList> player_temp;
    std::vector<PlayerList> player_list;
    std::vector<ESPData> data_temp;
    std::vector<ESPData> data_list;
};

auto esp = std::make_unique<ESP>();

namespace RenderLoop {
    ImDrawList* DrawList;
    float FontSize = 12.f;

    void DrawTextWithOutline(ImDrawList* draw_list, const std::string& text, const ImVec2& pos, ImU32 text_color, ImU32 outline_color, float font_size) {
        float offset = 1.0f;
        for (int x = -1; x <= 1; x++) {
            for (int y = -1; y <= 1; y++) {
                if (x == 0 && y == 0) continue;
                draw_list->AddText(ImGui::GetIO().FontDefault, font_size, ImVec2(pos.x + x * offset, pos.y + y * offset), outline_color, text.c_str());
            }
        }
        draw_list->AddText(ImGui::GetIO().FontDefault, font_size, pos, text_color, text.c_str());
    }

    void DrawAnimalESP(Vector3 itemPos, const std::string& item_string, ImU32 text_color, ImU32 outline_color) {
        Vector2 item_pos;
        if (!Camera::GetMainCamera()->WorldToScreen(itemPos, item_pos)) return;

        ImVec2 im_item_pos(item_pos.x, item_pos.y);
        DrawTextWithOutline(DrawList, item_string, im_item_pos, text_color, outline_color, FontSize);
    }

    void DrawBox(const BasePlayer::bounds_t& bounds, ImU32 color, bool outlined = false) {
        if (outlined) {
            DrawList->AddRect(ImVec2(bounds.left - 1, bounds.top - 1), ImVec2(bounds.right + 1, bounds.bottom + 1), IM_COL32(0, 0, 0, 255), 0.0f, 0, 1.5f);
            DrawList->AddRect(ImVec2(bounds.left + 1, bounds.top + 1), ImVec2(bounds.right - 1, bounds.bottom - 1), IM_COL32(0, 0, 0, 255), 0.0f, 0, 1.5f);
        }
        DrawList->AddRect(ImVec2(bounds.left, bounds.top), ImVec2(bounds.right, bounds.bottom), color, 0.0f, 0, 1.5f);
    }

    void DrawCorneredBox(const BasePlayer::bounds_t& bounds, ImU32 color, bool outlined = false) {
        float cornerSize = (bounds.right - bounds.left) * 0.2f;

        // Top-left corner
        DrawList->AddLine(ImVec2(bounds.left, bounds.top), ImVec2(bounds.left + cornerSize, bounds.top), color, 1.5f);
        DrawList->AddLine(ImVec2(bounds.left, bounds.top), ImVec2(bounds.left, bounds.top + cornerSize), color, 1.5f);

        // Top-right corner
        DrawList->AddLine(ImVec2(bounds.right, bounds.top), ImVec2(bounds.right - cornerSize, bounds.top), color, 1.5f);
        DrawList->AddLine(ImVec2(bounds.right, bounds.top), ImVec2(bounds.right, bounds.top + cornerSize), color, 1.5f);

        // Bottom-left corner
        DrawList->AddLine(ImVec2(bounds.left, bounds.bottom), ImVec2(bounds.left + cornerSize, bounds.bottom), color, 1.5f);
        DrawList->AddLine(ImVec2(bounds.left, bounds.bottom), ImVec2(bounds.left, bounds.bottom - cornerSize), color, 1.5f);

        // Bottom-right corner
        DrawList->AddLine(ImVec2(bounds.right, bounds.bottom), ImVec2(bounds.right - cornerSize, bounds.bottom), color, 1.5f);
        DrawList->AddLine(ImVec2(bounds.right, bounds.bottom), ImVec2(bounds.right, bounds.bottom - cornerSize), color, 1.5f);

        if (outlined) {
            cornerSize += 1.0f;
            // Outline for top-left corner
            DrawList->AddLine(ImVec2(bounds.left - 1, bounds.top - 1), ImVec2(bounds.left + cornerSize, bounds.top - 1), IM_COL32(0, 0, 0, 255), 1.5f);
            DrawList->AddLine(ImVec2(bounds.left - 1, bounds.top - 1), ImVec2(bounds.left - 1, bounds.top + cornerSize), IM_COL32(0, 0, 0, 255), 1.5f);

            // Outline for top-right corner
            DrawList->AddLine(ImVec2(bounds.right + 1, bounds.top - 1), ImVec2(bounds.right - cornerSize, bounds.top - 1), IM_COL32(0, 0, 0, 255), 1.5f);
            DrawList->AddLine(ImVec2(bounds.right + 1, bounds.top - 1), ImVec2(bounds.right + 1, bounds.top + cornerSize), IM_COL32(0, 0, 0, 255), 1.5f);

            // Outline for bottom-left corner
            DrawList->AddLine(ImVec2(bounds.left - 1, bounds.bottom + 1), ImVec2(bounds.left + cornerSize, bounds.bottom + 1), IM_COL32(0, 0, 0, 255), 1.5f);
            DrawList->AddLine(ImVec2(bounds.left - 1, bounds.bottom + 1), ImVec2(bounds.left - 1, bounds.bottom - cornerSize), IM_COL32(0, 0, 0, 255), 1.5f);

            // Outline for bottom-right corner
            DrawList->AddLine(ImVec2(bounds.right + 1, bounds.bottom + 1), ImVec2(bounds.right - cornerSize, bounds.bottom + 1), IM_COL32(0, 0, 0, 255), 1.5f);
            DrawList->AddLine(ImVec2(bounds.right + 1, bounds.bottom + 1), ImVec2(bounds.right + 1, bounds.bottom - cornerSize), IM_COL32(0, 0, 0, 255), 1.5f);
        }
    }

    void DrawPlayerName(const BasePlayer::bounds_t& bounds, const std::string& name, ImU32 color) {
        ImVec2 name_text_size = ImGui::CalcTextSize(name.c_str());
        float name_centered_x = bounds.left + (bounds.right - bounds.left) / 2.0f - (name_text_size.x / 2.0f);
        float name_y_pos = bounds.top - name_text_size.y - 2.0f;
        DrawTextWithOutline(DrawList, name, ImVec2(name_centered_x, name_y_pos), color, IM_COL32(0, 0, 0, 255), FontSize);
    }

    void DrawDistance(const BasePlayer::bounds_t& bounds, float distance, ImU32 color) {
        std::string distance_text = std::to_string(static_cast<int>(distance)) + "M";
        ImVec2 distance_text_size = ImGui::CalcTextSize(distance_text.c_str());
        float distance_centered_x = bounds.left + (bounds.right - bounds.left) / 2.0f - (distance_text_size.x / 2.0f);
        float distance_y_pos = bounds.bottom + 2.0f;
        DrawTextWithOutline(DrawList, distance_text, ImVec2(distance_centered_x, distance_y_pos), color, IM_COL32(0, 0, 0, 255), FontSize);
    }

    void DrawWeaponName(const BasePlayer::bounds_t& bounds, const std::string& weapon_name, ImU32 color) {
        std::string weapon_text = weapon_name;
        ImVec2 weapon_text_size = ImGui::CalcTextSize(weapon_text.c_str());
        float weapon_centered_x = bounds.left + (bounds.right - bounds.left) / 2.0f - (weapon_text_size.x / 2.0f);
        float weapon_y_pos = bounds.bottom + 2.0f;
        DrawTextWithOutline(DrawList, weapon_text, ImVec2(weapon_centered_x, weapon_y_pos), color, IM_COL32(0, 0, 0, 255), FontSize);
    }


    void DrawSkeleton(std::vector<std::pair<BoneList, Vector2>> validBones, ImU32 color, bool outlined = false) {
        if (validBones.size() < 2) return;

        ImDrawList* draw_list = ImGui::GetOverlayDrawList();
        if (!draw_list) return;

        static const std::vector<std::pair<int, int>> boneConnections = {
            {0, 1}, {1, 2}, {2, 6},
            {6, 7}, {7, 9}, {7, 12},
            {3, 4}, {4, 5},
            {8, 7}, {10, 11},
            {9, 10},
            {13, 12}, {14, 13},
            {5, 6}
        };

        for (const auto& connection : boneConnections) {
            int startBoneIndex = connection.first;
            int endBoneIndex = connection.second;

            if (startBoneIndex >= validBones.size() || endBoneIndex >= validBones.size()) continue;

            ImVec2 start = ImVec2(validBones[startBoneIndex].second.x, validBones[startBoneIndex].second.y);
            ImVec2 end = ImVec2(validBones[endBoneIndex].second.x, validBones[endBoneIndex].second.y);

            if (outlined) {
                draw_list->AddLine(ImVec2(start.x + 1, start.y + 1), ImVec2(end.x + 1, end.y + 1), IM_COL32(0, 0, 0, 255), 3.0f);
                draw_list->AddLine(ImVec2(start.x - 1, start.y - 1), ImVec2(end.x - 1, end.y - 1), IM_COL32(0, 0, 0, 255), 3.0f);
            }
            draw_list->AddLine(start, end, color, 1.0f);
        }
    }


    bool IsInFOV(Vector2 targetScreenPos, float fovSize, Vector2 screenCenter) {
        float dx = targetScreenPos.x - screenCenter.x;
        float dy = targetScreenPos.y - screenCenter.y;
        float distance = sqrt(dx * dx + dy * dy);
        return distance <= fovSize;
    }

    float DistanceToCenter(const Vector2& screenPos) {
        Vector2 screenCenter(Backend::width / 2, Backend::height / 2);
        return screenCenter.Distance(screenPos);
    }

    void RenderFOVCircle(float fovSize, bool isVisible) {
        if (isVisible) {
            ImVec2 screenCenter(Backend::width / 2, Backend::height / 2);
            DrawList->AddCircle(screenCenter, fovSize, ImColor(var::esp::FovColor[0], var::esp::FovColor[1], var::esp::FovColor[2], 1.0f), var::aim::fovsegments, var::aim::fovthickness);
        }
    }


    struct dynamic_array { uint64_t base; uint64_t mem_id; uint64_t sz; uint64_t cap; };

    Vector2 CalculateAngle(const Vector3& player, const Vector3& target)
    {
        Vector3 direction = Vector3(player.x - target.x, player.y - target.y, player.z - target.z);
        return Vector2(RAD2DEG(asin(direction.y / direction.Length())), RAD2DEG(-atan2(direction.x, -direction.z)));
    }

    Vector2 BezierCurve(Vector2 start, Vector2 control, Vector2 end, float t)
    {
        float u = 1.0f - t;
        float tt = t * t;
        float uu = u * u;

        Vector2 result = start * uu + control * (2.0f * u * t) + end * tt;
        return result;
    }

    void DrawArrow(ImVec2 center, Vector2 targetScreenPos, float radius, ImColor color, bool outlined = false) {
        Vector2 direction = Vector2(targetScreenPos.x - center.x, targetScreenPos.y - center.y);
        float distance = sqrt(direction.x * direction.x + direction.y * direction.y);
        if (distance < 0.001f) return;

        Vector2 normalizedDir(direction.x / distance, direction.y / distance);
        float angle = atan2(normalizedDir.y, normalizedDir.x);

        Vector2 arrowPos = Vector2(center.x + radius * normalizedDir.x, center.y + radius * normalizedDir.y);

        float arrowSize = 16.0f; // Smaller size for better performance
        ImVec2 p1(arrowPos.x, arrowPos.y);
        ImVec2 p2(arrowPos.x - arrowSize * cos(angle - 0.5f), arrowPos.y - arrowSize * sin(angle - 0.5f));
        ImVec2 p3(arrowPos.x - arrowSize * cos(angle + 0.5f), arrowPos.y - arrowSize * sin(angle + 0.5f));

        if (outlined) {
            // Outline triangle with slightly larger size
            float outlineSize = arrowSize + 1.5f;
            ImVec2 o1(arrowPos.x, arrowPos.y);
            ImVec2 o2(arrowPos.x - outlineSize * cos(angle - 0.5f), arrowPos.y - outlineSize * sin(angle - 0.5f));
            ImVec2 o3(arrowPos.x - outlineSize * cos(angle + 0.5f), arrowPos.y - outlineSize * sin(angle + 0.5f));
            DrawList->AddTriangleFilled(o1, o2, o3, IM_COL32(0, 0, 0, 255));
        }

        DrawList->AddTriangleFilled(p1, p2, p3, color);
    }

    bool IsOnMyTeam(BasePlayer* player) {
        if (!data->LocalPlayer || !player) return false;
        return player->teamID() == data->LocalPlayer->teamID();
    }

    inline void ProcessSkinnedMeshRenderer(uintptr_t renderer, int weaponmaterial) {
        if (renderer == 0) return;

        for (std::uint32_t idx{ 0 }; idx < 2; idx++) {
            const auto renderEntry = tRead<uintptr_t>(renderer + 0x20 + (idx * 0x8));
            if (!renderEntry) continue;

            const auto untity_object = tRead<uintptr_t>(renderEntry + 0x10);
            if (!untity_object) continue;

            const auto mat_list = tRead<dynamic_array>(untity_object + 0x148);
            if (mat_list.sz < 1 || mat_list.sz > 5) continue;

            for (std::uint32_t idx{ 0 }; idx < mat_list.sz; idx++) {
                tWrite<unsigned int>(mat_list.base + (idx * 0x4), weaponmaterial);
            }
        }
    }

    inline void GetComponentsInChildrenForWeapons(std::uintptr_t game_object,
        std::vector<std::uintptr_t>& gunRenderers) {

        const auto component_list = tRead<std::uint64_t>(game_object + 0x30);
        if (!component_list) return;

        const auto component_size = tRead<int>(game_object + 0x40);
        if (!component_size) return;

        // Traverse the components of the current object
        for (int idx{ 0 }; idx < component_size; ++idx) {
            if (component_size > 10000) return;
            const auto component = tRead<std::uint64_t>(component_list + (0x10 * idx + 0x8));
            if (!component) continue;

            // Check the name of the component (for MeshRenderer, SkinnedMeshRenderer, etc.)
            const auto component_ptr = tRead<std::uint64_t>(component + 0x28);
            if (!component_ptr) continue;

            const auto component_name_ptr = tRead<std::uint64_t>(component_ptr + 0x0);
            if (!component_name_ptr) continue;

            const auto component_name = tRead<std::uint64_t>(component_name_ptr + 0x10);
            if (!component_name) continue;

            auto name = readstring(component_name);
            if (name.empty()) continue;

            if (name == "SkinnedMeshRenderer" || name == "MeshRenderer") {
                gunRenderers.push_back(component);
            }

            if (name == "Transform") {
                const auto child_list = tRead<std::uint64_t>(component + 0x70);
                if (!child_list) continue;

                const auto child_size = tRead<int>(component + 0x80);
                if (!child_size) continue;

                for (int i{ 0 }; i < child_size; ++i) {
                    const auto child_transform = tRead<std::uint64_t>(child_list + (0x8 * i));
                    if (!child_transform) continue;

                    const auto child_game_object = tRead<std::uint64_t>(child_transform + 0x30);
                    if (!child_game_object) continue;

                    const auto child_object_name = tRead<std::uint64_t>(child_game_object + 0x60);
                    if (!child_object_name) continue;

                    const auto child_name = readstring(child_object_name);

                    if (child_name.find("holosight") != std::string::npos) continue;

                    GetComponentsInChildrenForWeapons(child_game_object, gunRenderers);
                }
            }
        }
    }

    inline void GetComponentsInChildren(std::uintptr_t game_object,
        std::vector<std::uintptr_t>& gunRenderers,
        std::vector<std::uintptr_t>& handRenderers,
        int depth = 0, bool weapon = false) {
        const auto component_list = tRead<std::uint64_t>(game_object + 0x30);
        if (!component_list) return;

        const auto component_size = tRead<int>(game_object + 0x40);
        if (!component_size) return;

        for (int idx{ 0 }; idx < component_size; ++idx) {
            if (component_size > 10000) return;
            const auto component = tRead<std::uint64_t>(component_list + (0x10 * idx + 0x8));
            if (!component) continue;

            const auto component_ptr = tRead<std::uint64_t>(component + 0x28);
            if (!component_ptr) continue;

            const auto component_name_ptr = tRead<std::uint64_t>(component_ptr + 0x0);
            if (!component_name_ptr) continue;

            const auto component_name = tRead<std::uint64_t>(component_name_ptr + 0x10);
            if (!component_name) continue;

            auto name = readstring(component_name);
            if (name.empty()) continue;

            if (name == "SkinnedMeshRenderer" || name == "MeshRenderer") {
                if (weapon) {
                    gunRenderers.push_back(component);
                }
                else {
                    handRenderers.push_back(component);
                }
            }

            if (name == "Transform") {
                const auto child_list = tRead<std::uint64_t>(component + 0x70);
                if (!child_list) continue;

                const auto child_size = tRead<int>(component + 0x80);
                if (!child_size) continue;

                for (int i{ 0 }; i < child_size; ++i) {
                    const auto child_transform = tRead<std::uint64_t>(child_list + (0x8 * i));
                    if (!child_transform) continue;

                    const auto child_game_object = tRead<std::uint64_t>(child_transform + 0x30);
                    if (!child_game_object) continue;

                    const auto child_object_name = tRead<std::uint64_t>(child_game_object + 0x60);
                    if (!child_object_name) continue;

                    const auto child_name = readstring(child_object_name);

                    if (child_name.find("holosight") != std::string::npos) continue;

                    bool isWeapon = child_name.rfind("v_", 0) == 0;


                    if (isWeapon) {
                        GetComponentsInChildrenForWeapons(child_game_object, gunRenderers);
                    }
                    else {
                        GetComponentsInChildren(child_game_object, gunRenderers, handRenderers, depth + 1, false);
                    }
                }
            }
        }
    }

    std::map<std::string, float> bullets =
    {
        { "ammo.rifle", 1.0f },
        { "ammo.rifle.hv", 1.2f },
        { "ammo.rifle.explosive", 0.49f },
        { "ammo.rifle.incendiary", 0.55f },
        { "ammo.pistol", 1.0f },
        { "ammo.pistol.hv", 1.33333f },
        { "ammo.pistol.fire", 0.75f },
        { "arrow.wooden", 1.0f },
        { "arrow.hv", 1.6f },
        { "arrow.fire", 0.8f },
        { "arrow.bone", 0.9f },
        { "ammo.handmade.shell", 1.0f },
        { "ammo.shotgun.slug", 2.25f },
        { "ammo.shotgun.fire", 1.0f },
        { "ammo.shotgun", 2.25f },
        { "ammo.nailgun.nails", 1.f }
    };

    float ProjectileSpeed_Normal(float dist, std::string currentweapon)
    {
        const float default_speed = 500.0f;
        auto BaseProjectilePrediction = tRead<uintptr_t>(reinterpret_cast<uintptr_t>(data->LocalPlayer->GetHeldItem()) + O::Projectile::HeldItem);

        uint64_t primary_magazine = tRead<uint64_t>(BaseProjectilePrediction + O::BaseProjectile::primaryMagazine);
        if (!primary_magazine)
            return default_speed;

        uint64_t AmmoType = tRead<uint64_t>(primary_magazine + 0x20);
        if (!AmmoType)
            return default_speed;

        uint64_t Unk3 = tRead<uint64_t>(AmmoType + 0x28);
        if (!Unk3)
            return default_speed;

        std::string magazine_shortname = read_wstr(Unk3 + 0x14);
        if (magazine_shortname.empty())
            return default_speed;

        float bullet_multiplier = bullets[magazine_shortname];
        auto HandWeapon = currentweapon.c_str();
        if (HandWeapon == std::string("rifle.ak") || HandWeapon == std::string("rifle.ak.ice") || HandWeapon == std::string("rifle.ak.diver"))
        {
            if (dist <= 0.f)
            {
                return 375.9f * bullet_multiplier;
            }
            else if (dist <= 150.f)
            {
                return 386.f * bullet_multiplier;
            }
            else if (dist <= 200.f)
            {
                return 390.f * bullet_multiplier;
            }
            else if (dist <= 250.f)
            {
                return 400.f * bullet_multiplier;
            }
            else if (dist <= 300.0f)
            {
                return 410.f * bullet_multiplier;
            }
        }
        if (HandWeapon == std::string("hmlmg")) return 500 * bullet_multiplier;
        if (HandWeapon == std::string("rifle.lr300")) return 300.f * bullet_multiplier;
        if (HandWeapon == std::string("rifle.bolt")) return 579.f * bullet_multiplier;
        if (HandWeapon == std::string("rifle.l96")) return 1126.0f * bullet_multiplier;
        if (HandWeapon == std::string("rifle.m39")) return 445.f * bullet_multiplier;
        if (HandWeapon == std::string("rifle.semiauto") || HandWeapon == std::string("rifle.sks")) return 358.0f * bullet_multiplier;
        if (HandWeapon == std::string("lmg.m249")) return 450.f * bullet_multiplier;
        if (HandWeapon == std::string("smg.thompson"))
        {
            if (dist <= 180.f)
            {
                return 270.f * bullet_multiplier;
            }
            else if (dist <= 204.f)
            {
                return 250.f * bullet_multiplier;
            }
            else if (dist <= 250.f)
            {
                return 245.f * bullet_multiplier;
            }
            else if (dist <= 350.f)
            {
                return 230.f * bullet_multiplier;
            }
        }
        if (HandWeapon == std::string("smg.2"))
        {
            if (dist <= 160.f)
            {
                return 220.f * bullet_multiplier;
            }
            else if (dist <= 200.f)
            {
                return 195.f * bullet_multiplier;
            }
        }
        if (HandWeapon == std::string("smg.mp5")) return 240.f * bullet_multiplier;
        if (HandWeapon == std::string("pistol.prototype17"))
        {
            if (dist <= 110.f)
            {
                return 300.f * bullet_multiplier;
            }
            else if (dist <= 169.f)
            {
                return 290.f * bullet_multiplier;
            }
            else if (dist <= 170.f)
            {
                return 280.f * bullet_multiplier;
            }
            else if (dist <= 180.f)
            {
                return 275.f * bullet_multiplier;
            }
            else if (dist <= 190.f)
            {
                return 270.f * bullet_multiplier;
            }
            else if (dist <= 200.f)
            {
                return 265.f * bullet_multiplier;
            }
        }
        if (HandWeapon == std::string("pistol.python")) return 300.f * bullet_multiplier;
        if (HandWeapon == std::string("pistol.semiauto")) return 300.f * bullet_multiplier;
        if (HandWeapon == std::string("pistol.revolver")) return 270.f * bullet_multiplier;
        if (HandWeapon == std::string("pistol.m92")) return 300.f * bullet_multiplier;
        if (HandWeapon == std::string("pistol.eoka")) return 90.f * bullet_multiplier;
        if (HandWeapon == std::string("pistol.nailgun"))
        {
            if (dist <= 60.f)
            {
                return 59.f * bullet_multiplier;
            }
            else if (dist <= 85.f)
            {
                return 58.3f * bullet_multiplier;
            }
            else
            {
                return 57.8f * bullet_multiplier;
            }
        }
        if (HandWeapon == std::string("crossbow"))
        {
            if (dist <= 83.f)
            {
                return 90.f * bullet_multiplier;
            }
            else if (dist <= 100.f)
            {
                return 88.f * bullet_multiplier;
            }
            else if (dist <= 150.f)
            {
                return 86.f * bullet_multiplier;
            }
            else
            {
                return 86.f * bullet_multiplier;
            }
        }
        if (HandWeapon == std::string("bow.compound"))
        {
            if (dist <= 90.f)
            {
                return 120.f * bullet_multiplier;
            }
            else if (dist <= 150.f)
            {
                return 115.76f * bullet_multiplier;
            }
            else
            {
                return 115.5f * bullet_multiplier;
            }
        }
        if (HandWeapon == std::string("bow.hunting") || HandWeapon == std::string("legacy bow"))
        {
            if (dist <= 41.f)
            {
                return 60.f * bullet_multiplier;
            }
            else if (dist <= 82.f)
            {
                return 58.f * bullet_multiplier;
            }
            else if (dist <= 102.f)
            {
                return 57.5f * bullet_multiplier;
            }
            else if (dist <= 112.f)
            {
                return 57.3f * bullet_multiplier;
            }
            else if (dist <= 127.f)
            {
                return 57.f * bullet_multiplier;
            }
            else if (dist <= 146.f)
            {
                return 56.5f * bullet_multiplier;
            }
            else if (dist <= 153.f)
            {
                return 56.3f * bullet_multiplier;
            }
            else if (dist <= 163.f)
            {
                return 56.f * bullet_multiplier;
            }
            else if (dist <= 172.f)
            {
                return 55.7f * bullet_multiplier;
            }
            else if (dist <= 178.f)
            {
                return 55.5f * bullet_multiplier;
            }
            else if (dist <= 184.f)
            {
                return 55.3f * bullet_multiplier;
            }
            else if (dist <= 189.f)
            {
                return 55.1f * bullet_multiplier;
            }
            else if (dist <= 196.f)
            {
                return 54.9f * bullet_multiplier;
            }
            else if (dist <= 201.f)
            {
                return 54.7f * bullet_multiplier;
            }
            else if (dist <= 206.f)
            {
                return 54.5f * bullet_multiplier;
            }
            else if (dist <= 210.f)
            {
                return 54.3f * bullet_multiplier;
            }
            else if (dist <= 215.f)
            {
                return 54.1f * bullet_multiplier;
            }
            else if (dist <= 220.f)
            {
                return 53.9f * bullet_multiplier;
            }
            else if (dist <= 225.1f)
            {
                return 53.7f * bullet_multiplier;
            }
            else if (dist <= 230.1f)
            {
                return 53.5f * bullet_multiplier;
            }
            else if (dist <= 233.1f)
            {
                return 53.3f * bullet_multiplier;
            }
            else if (dist <= 237.1f)
            {
                return 53.1f * bullet_multiplier;
            }
            else if (dist <= 241.1f)
            {
                return 52.9f * bullet_multiplier;
            }
            else if (dist <= 244.1f)
            {
                return 52.7f * bullet_multiplier;
            }
            else if (dist <= 248.1f)
            {
                return 52.5f * bullet_multiplier;
            }
            else if (dist <= 252.1f)
            {
                return 52.3f * bullet_multiplier;
            }
            else if (dist <= 255.1f)
            {
                return 52.1f * bullet_multiplier;
            }
            else if (dist <= 500.f)
            {
                return 50.f * bullet_multiplier;
            }
        }
        if (HandWeapon == std::string("shotgun.pump")) return 100.0f * bullet_multiplier;
        if (HandWeapon == std::string("shotgun.spas12")) return 100.0f * bullet_multiplier;
        if (HandWeapon == std::string("shotgun.waterpipe")) return 100.0f * bullet_multiplier;
        if (HandWeapon == std::string("shotgun.double")) return 100.0f * bullet_multiplier;
        return default_speed;
    }

    Vector3 Prediction(Vector3 LP_Pos, Vector3 Velocity, Vector3 BonePos, std::string HandWeapon) {
        const float Dist = LP_Pos.distance(BonePos);
        if (Dist > 0.001f) {
            const float BulletTime = Dist / ProjectileSpeed_Normal(Dist, HandWeapon.c_str());
            const Vector3 vel = Velocity;
            Vector3 PredictVel;
            PredictVel.x = vel.x * BulletTime * 0.75f;
            PredictVel.y = vel.y * BulletTime * 0.75f;
            PredictVel.z = vel.z * BulletTime * 0.75f;

            BonePos.x += PredictVel.x;
            BonePos.y += PredictVel.y;
            BonePos.z += PredictVel.z;
            BonePos.y += (4.905f * BulletTime * BulletTime);
        } return BonePos;
    }

    void RenderFOVArrows() {
        if (!var::esp::fovarrows) return;

        ImVec2 screenCenter(Backend::width / 2, Backend::height / 2);
        float fovRadius = var::aim::fovsize + 25.0f;

        struct ArrowTarget {
            Vector2 targetScreenPos;
            float distance;
            ImColor color;
        };

        std::vector<ArrowTarget> targets;
        targets.reserve(esp->player_list.size());

        for (auto& player : esp->player_list) {
            if (!player.ptr || (IsOnMyTeam(player.ptr) && var::esp::TeamCheck)) continue;

            Vector3 headPos = player.ptr->GetBoneLocation(47);
            Vector2 targetScreenPos;

            if (!Camera::GetMainCamera()->WorldToScreen(headPos, targetScreenPos)) continue;

            float distanceToCenter = DistanceToCenter(targetScreenPos);
            if (distanceToCenter <= fovRadius) continue;

            ImColor arrowColor;

            if (static_cast<int>(player.ptr->playerFlags()) & static_cast<int>(PlayerFlags::Sleeping)) {
                arrowColor = ImColor(var::esp::SleeperColor[0], var::esp::SleeperColor[1], var::esp::SleeperColor[2], 1.0f);
            }
            else if (static_cast<int>(player.ptr->playerFlags()) & static_cast<int>(PlayerFlags::Wounded)) {
                arrowColor = ImColor(var::esp::WoundedColor[0], var::esp::WoundedColor[1], var::esp::WoundedColor[2], 1.0f);
            }
            else if (player.ptr->_displayName()->Get().find(oxorany("scientist")) != std::string::npos ||
                player.ptr->_displayName()->Get().find(oxorany("Scientist")) != std::string::npos) {
                arrowColor = ImColor(var::esp::ScientistColor[0], var::esp::ScientistColor[1], var::esp::ScientistColor[2], 1.0f);
            }
            else {
                arrowColor = ImColor(var::esp::VisualsColor[0], var::esp::VisualsColor[1], var::esp::VisualsColor[2], 1.0f);
            }

            targets.push_back({ targetScreenPos, distanceToCenter, arrowColor });
        }

        const size_t maxArrows = std::min<size_t>(targets.size(), 10);
        if (maxArrows == 0) return;

        std::partial_sort(targets.begin(), targets.begin() + maxArrows, targets.end(),
            [](const ArrowTarget& a, const ArrowTarget& b) {
                return a.distance < b.distance;
            });

        for (size_t i = 0; i < maxArrows; i++) {
            DrawArrow(screenCenter, targets[i].targetScreenPos, fovRadius, targets[i].color, var::esp::Outlined);
        }
    }

    std::string getWeaponKey(const std::string& weaponName) {
        std::string key = weaponName;
        for (char& ch : key) {
            if (ch == '.') ch = '_';
        }
        return key;
    }

    Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
        return Vector3(
            a.x + (b.x - a.x) * t,
            a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t
        );
    }

    Vector2 NormalizeAngle(Vector2 angle)
    {
        if (angle.x > 180.0f) angle.x -= 360.0f;
        if (angle.x < -180.0f) angle.x += 360.0f;
        if (angle.y > 180.0f) angle.y -= 360.0f;
        if (angle.y < -180.0f) angle.y += 360.0f;

        return angle;
    }

    struct RecoilData {
        std::array<float, 4> RecoilProperties;
        std::array<float, 4> NewRecoilOverride;
    };

    std::unordered_map<std::string, RecoilData> recoilMap = {
        {("rifle_ak"), {{-2, 8, -4, -30}, {1.5, 2.5, -2.5, -3.5}}},
        {("rifle_ak_ice"), {{-2, 8, -4, -30}, {1.5, 2.5, -2.5, -3.5}}},
        {("rifle_ak_diver"), {{-2, 8, -4, -30}, {1.5, 2.5, -2.5, -3.5}}},
        {("rifle_ak_med"), {{-2, 8, -4, -30}, {1.5, 2.5, -2.5, -3.5}}},
        {("rifle_lr300"), {{-1, 5, -2.5, -12}, {-0.5, 0.5, -2, -3}}},
        {("smg_2"), {{-1.5, 10, -2, -15}, {-1, 1, -1.5, -2}}},
        {("revolver_hc"), {{-2, 2, -15, -16}, {0, 0, 0, 0}}},
        {("rifle_sks"), {{-1, 1, -5, -6}, {-0.3, 0.3, -3, -3.5}}},
        {("t1_smg"), {{-1.5, 10, -2, -15}, {-1, 1, -2, -2.5}}},
        {("pistol_python"), {{-2, 2, -15, -16}, {0, 0, 0, 0}}},
        {("pistol_semiauto"), {{-2, 2, -6, -8}, {-1, 1, -2, -2.5}}},
        {("shotgun_m4"), {{2, 4, -7, -10}, {0, 0, 0, 0}}},
        {("lmg_m249"), {{-1, 1, -5, -6}, {1.25, 2.25, -3, -4}}},
        {("rifle_m39"), {{-1.5, 1.5, -5, -7}, {1.5, 2.5, -3, -4}}},
        {("rifle_semiauto"), {{-1, 1, -5, -6}, {-0.5, 0.5, -2, -3}}},
        {("rifle_bolt"), {{-4, 4, -2, -3}, {0, 0, 0, 0}}},
        {("rifle_l96"), {{-2, 2, -1, -1.5}, {0, 0, 0, 0}}},
        {("hmlmg"), {{0.5, 0.75, -3, -4}, {-1.25, -2.5, -3, -4}}},
        {("minigun"), {{0, 0, -0.5, -1}, {0, 0, 0, 0}}},
        {("pistol_m92"), {{-1, 1, -7, -8}, {-1, 1, -7, -8}}},
        {("pistol_revolver"), {{-1, 1, -3, -6}, {0, 0, 0, 0}}},
        {("smg_thompson"), {{-1.5, 10, -2, -15}, {-1, 1, -1.5, -2}}},
        {("pistol_prototype17"), {{-1, 1, -2, -2.5}, {0, 0, 0, 0}}},
        {("smg_mp5"), {{-1.25, 6, -2, -10}, {-1, 1, -1, -3}}},
        {("bow_hunting"), {{-3, 3, -3, -6}, {0, 0, 0, 0}}},
        {("crossbow"), {{-3, 3, -3, -6}, {0, 0, 0, 0}}},
        {("bow_compound"), {{-3, 3, -3, -6}, {0, 0, 0, 0}}},
        {("shotgun_pump"), {{4, 8, -10, -14}, {0, 0, 0, 0}}},
        {("pistol_nailgun"), {{-1, 1, -3, -6}, {0, 0, 0, 0}}}
    };

    void Range(float* X, float* Y, float range)
    {
        if (abs((*X)) > range || abs((*Y)) > range)
        {
            if ((*Y) > (*X))
            {
                if ((*Y) > -(*X))
                {
                    (*X) = range * (*X) / (*Y);
                    (*Y) = range;
                }
                else
                {
                    (*Y) = -range * (*Y) / (*X);
                    (*X) = -range;
                }
            }
            else
            {
                if ((*Y) > -(*X))
                {
                    (*Y) = range * (*Y) / (*X);
                    (*X) = range;
                }
                else
                {
                    (*X) = -range * (*X) / (*Y);
                    (*Y) = -range;
                }
            }
        }
    }

    void CalcRadarPoint(Vector3 vOrigin, int& screenx, int& screeny)
    {
        if (!data->LocalPlayer) return;

        Vector3 cameraPos = Camera::GetMainCamera()->GetCameraLocation();
        float dx = vOrigin.x - cameraPos.x;
        float dz = vOrigin.z - cameraPos.z;
        float actualDistance = sqrtf(dx * dx + dz * dz);

        Matrix viewMatrix = Camera::GetMainCamera()->GetViewMatrix().transpose();
        float cameraYaw = atan2f(viewMatrix._31, viewMatrix._33);

        float cosYaw = cosf(cameraYaw);
        float sinYaw = sinf(cameraYaw);

        float x = dx * cosYaw - dz * sinYaw;
        float y = dx * sinYaw + dz * cosYaw;

        int radarX = var::esp::RadarX;
        int radarY = var::esp::RadarY;
        int radarCenterX = radarX + 100;
        int radarCenterY = radarY + 100;
        float radarRadius = 95.0f;
        float maxRange = 150.0f;

        float normalizedDist = (actualDistance / maxRange);
        if (normalizedDist > 1.0f) normalizedDist = 1.0f;

        float dirX = 0.0f, dirY = 0.0f;
        if (actualDistance > 0.1f) {
            dirX = y / actualDistance;  // Swap X and Y to fix orientation
            dirY = -x / actualDistance; // Negate X to correct direction
        }

        float scaledRadius = normalizedDist * radarRadius;
        screenx = radarCenterX + int(dirX * scaledRadius);
        screeny = radarCenterY + int(dirY * scaledRadius);

        float distFromCenter = sqrtf(powf(screenx - radarCenterX, 2) + powf(screeny - radarCenterY, 2));
        if (distFromCenter > radarRadius) {
            float angle = atan2f(screeny - radarCenterY, screenx - radarCenterX);
            screenx = radarCenterX + int(cosf(angle) * radarRadius);
            screeny = radarCenterY + int(sinf(angle) * radarRadius);
        }
    }


    Vector2 RotatePoint(Vector2 radar_pos, Vector2 radar_size, Vector2 LocalLocation, Vector2 TargetLocation)
    {
        auto dx = TargetLocation.x - LocalLocation.x;
        auto dy = TargetLocation.y - LocalLocation.y;

        auto X = -dy;
        auto Y = dx;

        float calculated_range = 34 * 1000;

        Range(&X, &Y, calculated_range);

        int rad_x = (int)radar_pos.x;
        int rad_y = (int)radar_pos.y;

        float r_siz_x = radar_size.x;
        float r_siz_y = radar_size.y;

        int x_max = (int)r_siz_x + rad_x - 5;
        int y_max = (int)r_siz_y + rad_y - 5;

        Vector2 return_value;
        return_value.x = rad_x + ((int)r_siz_x / 2 + int(X / calculated_range * r_siz_x));
        return_value.y = rad_y + ((int)r_siz_y / 2 + int(Y / calculated_range * r_siz_y));

        if (return_value.x > x_max)
            return_value.x = x_max;
        if (return_value.x < rad_x)
            return_value.x = rad_x;
        if (return_value.y > y_max)
            return_value.y = y_max;
        if (return_value.y < rad_y)
            return_value.y = rad_y;

        return return_value;
    }

    void DrawRadar()
    {
        if (!var::esp::Radar) return;
        if (!data->LocalPlayer) return;

        ImVec2 center = ImVec2(var::esp::RadarX + 100, var::esp::RadarY + 100);
        float radius = 100;

        ImVec4 accentColorVec = ImVec4(
            var::esp::VisualsColor[0],
            var::esp::VisualsColor[1],
            var::esp::VisualsColor[2],
            1.0f
        );

        ImU32 innerColor = ImGui::GetColorU32(ImVec4(0.1f, 0.1f, 0.1f, 0.9f));
        ImU32 lineColor = ImGui::GetColorU32(ImVec4(accentColorVec.x, accentColorVec.y, accentColorVec.z, 0.2f));
        ImU32 circleColor = ImGui::GetColorU32(ImVec4(accentColorVec.x, accentColorVec.y, accentColorVec.z, 0.1f));
        ImU32 borderColor = ImGui::GetColorU32(ImVec4(accentColorVec.x, accentColorVec.y, accentColorVec.z, 0.7f));

        ImGui::GetOverlayDrawList()->AddCircleFilled(center, radius, innerColor, 64);

        float time = ImGui::GetTime();
        float pulseStrength = (sinf(time * 2.0f) * 0.15f) + 0.85f;
        ImU32 glowColor = ImGui::GetColorU32(ImVec4(accentColorVec.x, accentColorVec.y, accentColorVec.z, 0.3f * pulseStrength));

        ImGui::GetOverlayDrawList()->AddCircle(center, radius * pulseStrength, glowColor, 64, 2.0f);
        ImGui::GetOverlayDrawList()->AddCircle(center, radius, borderColor, 64, 1.5f);

        std::array<float, 3> rangeCircles = { 0.33f, 0.66f, 1.0f };

        for (int i = 0; i < rangeCircles.size(); ++i)
        {
            float circleRadius = radius * rangeCircles[i];
            ImGui::GetOverlayDrawList()->AddCircle(center, circleRadius, circleColor, 32, 1.0f);
        }

        // Fixed direction labels - these now correspond to real world directions
        const int numLines = 8;
        const char* directions[8] = { "N", "NE", "E", "SE", "S", "SW", "W", "NW" };

        for (int i = 0; i < numLines; ++i)
        {
            float angle = (M_PI * 2.0f * i) / numLines;
            ImVec2 start = ImVec2(center.x + cosf(angle) * (radius * 0.1f), center.y + sinf(angle) * (radius * 0.1f));
            ImVec2 end = ImVec2(center.x + cosf(angle) * radius, center.y + sinf(angle) * radius);
            ImGui::GetOverlayDrawList()->AddLine(start, end, lineColor, 1.0f);

            ImVec2 textPos = ImVec2(
                center.x + cosf(angle) * (radius * 1.1f) - ImGui::CalcTextSize(directions[i]).x / 2,
                center.y + sinf(angle) * (radius * 1.1f) - ImGui::CalcTextSize(directions[i]).y / 2
            );

            // Add text with simple black outline
            ImGui::GetOverlayDrawList()->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 255), directions[i]);
            ImGui::GetOverlayDrawList()->AddText(textPos, lineColor, directions[i]);
        }

        // Get player's rotation angle from the camera matrix
        Matrix viewMatrix = Camera::GetMainCamera()->GetViewMatrix().transpose();
        float cameraYaw = atan2f(viewMatrix._31, viewMatrix._33);

        // This rotation needs to be inverted to match the game's coordinate system
        cameraYaw = -cameraYaw;

        float dirX = cosf(cameraYaw);
        float dirY = sinf(cameraYaw);

        float coneWidth = 0.3f;

        ImDrawList* drawList = ImGui::GetOverlayDrawList();
        drawList->PathClear();
        drawList->PathLineTo(center);

        // Draw the vision cone
        const int segments = 12;
        float stepAngle = coneWidth * 2.0f / segments;

        for (int i = 0; i <= segments; i++) {
            float segmentAngle = cameraYaw - coneWidth + stepAngle * i;
            float x = center.x + cosf(segmentAngle) * radius;
            float y = center.y + sinf(segmentAngle) * radius;
            drawList->PathLineTo(ImVec2(x, y));
        }

        drawList->PathFillConvex(ImGui::GetColorU32(ImVec4(
            accentColorVec.x, accentColorVec.y, accentColorVec.z, 0.15f * pulseStrength
        )));

        // Draw vision cone lines
        ImU32 beamColor = ImGui::GetColorU32(ImVec4(
            accentColorVec.x, accentColorVec.y, accentColorVec.z, 0.5f * pulseStrength
        ));

        ImVec2 leftBeam = ImVec2(
            center.x + cosf(cameraYaw - coneWidth) * radius,
            center.y + sinf(cameraYaw - coneWidth) * radius
        );

        ImVec2 rightBeam = ImVec2(
            center.x + cosf(cameraYaw + coneWidth) * radius,
            center.y + sinf(cameraYaw + coneWidth) * radius
        );

        drawList->AddLine(center, leftBeam, beamColor, 1.5f);
        drawList->AddLine(center, rightBeam, beamColor, 1.5f);

        // Draw local player
        float playerCircleRadius = 4.0f;
        ImU32 playerColor = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::GetOverlayDrawList()->AddCircleFilled(center, playerCircleRadius, playerColor, 12);

        int playersDisplayed = 0;

        for (auto& player : esp->player_list) {
            if (!player.ptr || player.ptr == data->LocalPlayer) continue;
            if (IsOnMyTeam(player.ptr) && var::esp::TeamCheck) continue;

            PlayerFlags flags = player.ptr->playerFlags();
            uint32_t flagsValue = static_cast<uint32_t>(flags);
            bool isSleeping = (flagsValue & static_cast<uint32_t>(PlayerFlags::Sleeping));
            bool isWounded = (flagsValue & static_cast<uint32_t>(PlayerFlags::Wounded));
            bool isScientist = (player.ptr->_displayName()->Get().find(oxorany("scientist")) != std::string::npos ||
                player.ptr->_displayName()->Get().find(oxorany("Scientist")) != std::string::npos);

            if ((var::esp::SkipSleepers && isSleeping) ||
                (var::esp::SkipKnocked && isWounded) ||
                (isScientist && !var::esp::show_scientists)) continue;

            Vector3 playerPos;
            bool validPosition = false;

            const int bonesToTry[] = { 47, 48, head, 0 };
            for (int boneIndex : bonesToTry) {
                playerPos = player.ptr->GetBoneLocation(boneIndex);
                if (!playerPos.empty() && !playerPos.Zero()) {
                    validPosition = true;
                    break;
                }
            }

            if (!validPosition) {
                playerPos = player.position;
                if (playerPos.empty() || playerPos.Zero()) continue;
            }

            int screenX, screenY;
            CalcRadarPoint(playerPos, screenX, screenY);

            Vector3 localPlayerPos = data->LocalPlayer->GetBoneLocation(0);
            float distToPlayer = localPlayerPos.distance(playerPos);
            float dotSize = 5.0f;

            ImColor playerColor;

            if (isSleeping) {
                playerColor = ImColor(var::esp::SleeperColor[0], var::esp::SleeperColor[1], var::esp::SleeperColor[2], 1.0f);
            }
            else if (isWounded) {
                playerColor = ImColor(var::esp::WoundedColor[0], var::esp::WoundedColor[1], var::esp::WoundedColor[2], 1.0f);
            }
            else if (isScientist) {
                playerColor = ImColor(var::esp::ScientistColor[0], var::esp::ScientistColor[1], var::esp::ScientistColor[2], 1.0f);
            }
            else {
                playerColor = ImColor(var::esp::VisualsColor[0], var::esp::VisualsColor[1], var::esp::VisualsColor[2], 1.0f);
            }

            ImGui::GetOverlayDrawList()->AddCircleFilled(
                ImVec2((float)screenX, (float)screenY),
                dotSize,
                playerColor,
                8
            );

            float glowPulse = sinf(time * 3.0f + distToPlayer * 0.05f) * 0.7f;
            ImGui::GetOverlayDrawList()->AddCircle(
                ImVec2((float)screenX, (float)screenY),
                dotSize + 1.0f + glowPulse,
                ImGui::GetColorU32(ImVec4(playerColor.Value.x, playerColor.Value.y, playerColor.Value.z, 0.5f * pulseStrength)),
                8,
                1.0f
            );

            if (var::esp::usernames_radar) {
                std::string displayName = player.ptr->_displayName()->Get();
                ImVec2 nameSize = ImGui::CalcTextSize(displayName.c_str());
                ImVec2 namePos = ImVec2((float)screenX - nameSize.x / 2, (float)screenY - dotSize - nameSize.y - 2);

                // Use simple black outline for text
                drawList->AddText(ImVec2(namePos.x + 1, namePos.y + 1), IM_COL32(0, 0, 0, 255), displayName.c_str());
                drawList->AddText(namePos, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), displayName.c_str());
            }

            if (var::esp::distance_radar) {
                int distance = static_cast<int>(distToPlayer);
                std::string distText = std::to_string(distance) + oxorany("m");
                ImVec2 distSize = ImGui::CalcTextSize(distText.c_str());
                ImVec2 distPos = ImVec2((float)screenX - distSize.x / 2, (float)screenY + dotSize + 2);

                // Use simple black outline for text
                drawList->AddText(ImVec2(distPos.x + 1, distPos.y + 1), IM_COL32(0, 0, 0, 255), distText.c_str());
                drawList->AddText(distPos, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), distText.c_str());
            }

            playersDisplayed++;
        }

        std::string debugText = oxorany("Players: ") + std::to_string(playersDisplayed);
        ImVec2 textPos = ImVec2(var::esp::RadarX + 5, var::esp::RadarY + radius * 2 + 5);
        drawList->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 255), debugText.c_str());
        drawList->AddText(textPos, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), debugText.c_str());
    }

    void Run() {
        DrawList = ImGui::GetOverlayDrawList();
        if (!DrawList) return;

        if (var::menu::Watermark) {
            auto watermark = oxorany("firox base | @gworld. | firox.lol Rust | Developer");
            auto fps_text = std::format(" | FPS: {:.1f}", ImGui::GetIO().Framerate);
            std::string full_text = watermark + fps_text;
            auto full_text_size = ImGui::CalcTextSize(full_text.c_str());
            ImVec2 rect_min(6, 9), rect_max(6 + full_text_size.x + 12, 9 + full_text_size.y + 8);
            DrawList->AddRectFilled(rect_min, ImVec2(rect_max.x, rect_min.y + 2), ImColor(178, 102, 255));
            DrawList->AddRectFilled(ImVec2(rect_min.x, rect_min.y + 2), rect_max, ImColor(15, 15, 15));
            DrawList->AddText(ImVec2(rect_min.x + 6, rect_min.y + 4), ImColor(255, 255, 255, 255), full_text.c_str());
        }

        constexpr uint32_t SLEEPING_FLAG = static_cast<uint32_t>(PlayerFlags::Sleeping);
        constexpr uint32_t WOUNDED_FLAG = static_cast<uint32_t>(PlayerFlags::Wounded);

        for (const auto& player : esp->player_list) {
            if (!player.ptr) continue;

            uint32_t flags = static_cast<uint32_t>(player.ptr->playerFlags());
            bool isSleeping = flags & SLEEPING_FLAG;
            bool isWounded = flags & WOUNDED_FLAG;
            bool isScientist = player.ptr->_displayName()->Get().find(oxorany("scientist")) != std::string::npos;

            if ((IsOnMyTeam(player.ptr) && var::esp::TeamCheck) ||
                (var::esp::SkipSleepers && isSleeping) ||
                (var::esp::SkipKnocked && isWounded)) continue;

            auto PlayerBound = player.ptr->GetBounds(4);
            Vector3 headPos = player.ptr->GetBoneLocation(47);
            Vector2 screenPos;
            if (!Camera::GetMainCamera()->WorldToScreen(headPos, screenPos)) continue;

            float distanceToCenter = DistanceToCenter(screenPos);
            ImU32 EspColor = isSleeping ?
                ImColor(var::esp::SleeperColor[0], var::esp::SleeperColor[1], var::esp::SleeperColor[2], 1.0f) :
                isWounded ?
                ImColor(var::esp::WoundedColor[0], var::esp::WoundedColor[1], var::esp::WoundedColor[2], 1.0f) :
                isScientist ?
                ImColor(var::esp::ScientistColor[0], var::esp::ScientistColor[1], var::esp::ScientistColor[2], 1.0f) :
                ImColor(var::esp::VisualsColor[0], var::esp::VisualsColor[1], var::esp::VisualsColor[2], 1.0f);

            if (var::esp::Box) DrawBox(PlayerBound, EspColor, var::esp::Outlined);
            if (var::esp::CornerBox) DrawCorneredBox(PlayerBound, EspColor, var::esp::Outlined);
            if (var::esp::Name) DrawPlayerName(PlayerBound, player.ptr->_displayName()->Get(), EspColor);
            if (var::esp::Distance) DrawDistance(PlayerBound, distanceToCenter, EspColor);
            if (var::esp::Weapon) DrawWeaponName(PlayerBound, player.ptr->GetHeldItem()->get_short_name(), EspColor);
            if (var::aim::fovvisible) RenderFOVCircle(var::aim::fovsize, var::aim::fovvisible);
            if (var::esp::Radar) DrawRadar();
            if (var::esp::fovarrows) RenderFOVArrows();

            if (var::esp::Skeleton) {
                auto validBones = player.ptr->GetValidBones();
                if (!validBones.empty()) DrawSkeleton(validBones, EspColor, var::esp::Outlined);
            }

            if (var::esp::HeadCircle && Camera::GetMainCamera()->WorldToScreen(headPos, screenPos)) {
                float headCircleRadius = (var::esp::HeadCircleSize * (PlayerBound.bottom - PlayerBound.top)) * 0.005f;
                ImVec2 screenPosIm(screenPos.x, screenPos.y);

                if (var::esp::Outlined) {
                    DrawList->AddCircle(screenPosIm, headCircleRadius + 1.0f, IM_COL32(0, 0, 0, 255));
                    DrawList->AddCircle(screenPosIm, headCircleRadius - 1.0f, IM_COL32(0, 0, 0, 255));
                }
                DrawList->AddCircle(screenPosIm, headCircleRadius, EspColor);
            }


        }
    }

    void AimbotLoop() {
        DrawList = ImGui::GetOverlayDrawList();
        if (!DrawList) return;

        static BasePlayer* currentTarget = nullptr;
        const Vector2 screenCenter(Backend::width * 0.5f, Backend::height * 0.5f);
        float closestDistance = FLT_MAX;
        BasePlayer* visibleTarget = nullptr;
        const std::string WeaponNameReal = data->LocalPlayer ? data->LocalPlayer->GetHeldItem()->get_short_name() : "";
        constexpr uint32_t SLEEPING_FLAG = static_cast<uint32_t>(PlayerFlags::Sleeping);
        constexpr uint32_t WOUNDED_FLAG = static_cast<uint32_t>(PlayerFlags::Wounded);

        // Find the best target first
        for (auto& potentialTarget : esp->player_list) {
            if (!potentialTarget.ptr || (IsOnMyTeam(potentialTarget.ptr) && var::esp::TeamCheck)) continue;

            uint32_t flags = static_cast<uint32_t>(potentialTarget.ptr->playerFlags());
            if ((var::esp::SkipSleepers && (flags & SLEEPING_FLAG)) ||
                (var::esp::SkipKnocked && (flags & WOUNDED_FLAG))) continue;

            Vector3 bonePos = potentialTarget.ptr->GetBoneLocation(47);
            Vector2 screenPos;
            if (Camera::GetMainCamera()->WorldToScreen(bonePos, screenPos)) {
                float distance = DistanceToCenter(screenPos);
                if (distance < closestDistance && IsInFOV(screenPos, var::aim::fovsize, screenCenter)) {
                    closestDistance = distance;
                    visibleTarget = potentialTarget.ptr;
                }
            }
        }

        // Draw target line if we have a target
        if (visibleTarget) {
            Vector3 targetHeadPos = visibleTarget->GetBoneLocation(47);

            if (var::aim::Prediction && data->LocalPlayer) {
                auto model = tRead<uintptr_t>(visibleTarget->Address() + O::BasePlayer::PlayerModel);
                auto velocity = tRead<Vector3>(model + O::PlayerModel::new_velocity);
                Vector3 cameraPos = Camera::GetMainCamera()->GetCameraLocation();
                targetHeadPos = Prediction(cameraPos, velocity, targetHeadPos, WeaponNameReal.c_str());
            }

            Vector2 targetScreenPos;
            if (Camera::GetMainCamera()->WorldToScreen(targetHeadPos, targetScreenPos) &&
                IsInFOV(targetScreenPos, var::aim::fovsize, screenCenter) &&
                var::aim::targetline) {
                ImGui::GetOverlayDrawList()->AddLine(
                    ImVec2(screenCenter.x, screenCenter.y),
                    ImVec2(targetScreenPos.x, targetScreenPos.y),
                    ImColor(var::esp::VisualsColor[0], var::esp::VisualsColor[1], var::esp::VisualsColor[2], 1.0f)
                );
            }
        }

        // Handle aiming when aim key is pressed
        if (var::aim::Enable && GetAsyncKeyState(var::aim::Key) & 0x8000) {
            if (currentTarget == nullptr) {
                currentTarget = visibleTarget;
            }

            if (currentTarget && data->LocalPlayer) {
                Vector3 targetHeadPos = currentTarget->GetBoneLocation(47);

                if (var::aim::Prediction) {
                    auto model = tRead<uintptr_t>(currentTarget->Address() + O::BasePlayer::PlayerModel);
                    auto velocity = tRead<Vector3>(model + O::PlayerModel::new_velocity);
                    Vector3 cameraPos = Camera::GetMainCamera()->GetCameraLocation();
                    targetHeadPos = Prediction(cameraPos, velocity, targetHeadPos, WeaponNameReal.c_str());
                }

                Vector2 targetScreenPos;
                if (Camera::GetMainCamera()->WorldToScreen(targetHeadPos, targetScreenPos) &&
                    IsInFOV(targetScreenPos, var::aim::fovsize, screenCenter)) {

                    Vector3 localPos = data->LocalPlayer->GetTransform(47)->GetBonePosition();
                    Vector2 angle = CalculateAngle(localPos, targetHeadPos);

                    if (!var::aim::silentaim && !var::aim::Smoothness) {
                        data->LocalPlayer->SetInputOffset(Vector3(angle.x, angle.y, 0.0f));
                    }
                    else if (var::aim::silentaim) {
                        data->LocalPlayer->GetEyes()->SetBodyRotation(ToQuat(Vector3(angle.x, angle.y, 0.0f)));
                    }
                    else if (!var::aim::silentaim && var::aim::Smoothness) {
                        Vector2 currentOffset(data->LocalPlayer->GetInputOffset().x, data->LocalPlayer->GetInputOffset().y);
                        Vector2 normCurrentAngle = NormalizeAngle(currentOffset);
                        Vector2 normTargetAngle = NormalizeAngle(angle);
                        Vector2 delta = NormalizeAngle(normTargetAngle - normCurrentAngle);
                        Vector2 controlPoint = normCurrentAngle + delta * 0.5f;
                        float t = 0.1f / var::aim::Smooth;
                        Vector2 smoothedAngle = BezierCurve(normCurrentAngle, controlPoint, normTargetAngle, t);
                        data->LocalPlayer->SetInputOffset(Vector3(smoothedAngle.x, smoothedAngle.y, 0.0f));
                    }
                }
            }
        }
        else {
            currentTarget = nullptr;
        }

        auto Movement = tRead<uintptr_t>(reinterpret_cast<uintptr_t>(data->LocalPlayer) + O::BasePlayer::PlayerMovement);

         if (var::esp::exploits::spiderman && data->LocalPlayer) {
            tWrite<float>(Movement + 0x108, 0.0f);
         }

        if (var::esp::exploits::fake_admin || var::esp::exploits::thirdperson) {
            auto playerFlags = tRead<int>(reinterpret_cast<uintptr_t>(data->LocalPlayer) + O::BasePlayer::playerFlags);
            if (playerFlags != 64 && playerFlags != 16) {
                if (var::esp::exploits::fake_admin && playerFlags != 4) {
                    tWrite<int>(reinterpret_cast<uintptr_t>(data->LocalPlayer) + O::BasePlayer::playerFlags, 4);
                }
                if (var::esp::exploits::thirdperson && playerFlags != 1024) {
                    tWrite<int>(reinterpret_cast<uintptr_t>(data->LocalPlayer) + O::BasePlayer::playerFlags, 1024);
                }
            }
        }
    }

    void WorldEsp() {
        if (var::esp::world::enable || var::esp::deployables::enable) {
        bool hasPlayers = false;

        for (auto& player : esp->player_list) {
            hasPlayers = true;

                for (auto& entity : esp->data_list) {
                    if (var::esp::world::enable) {
                        if (entity.gamebuff.find(oxorany("bear.prefab")) != std::string::npos && var::esp::world::bear) {
                            std::string item_string = oxorany("Bear");
                            Vector3 itemPos = entity.player->GetBoneLocation(0);
                            DrawAnimalESP(itemPos, item_string, ImColor(255, 255, 255), ImColor(77, 50, 25));
                        }

                        if (entity.gamebuff.find(oxorany("boar.prefab")) != std::string::npos && var::esp::world::boar) {
                            std::string item_string = oxorany("Boar");
                            Vector3 itemPos = entity.player->GetBoneLocation(0);
                            DrawAnimalESP(itemPos, item_string, ImColor(255, 255, 255), ImColor(33, 22, 11));
                        }

                        if (entity.gamebuff.find(oxorany("deer.prefab")) != std::string::npos && var::esp::world::deer) {
                            std::string item_string = oxorany("Deer");
                            Vector3 itemPos = entity.player->GetBoneLocation(0);
                            DrawAnimalESP(itemPos, item_string, ImColor(255, 255, 255), ImColor(122, 89, 56));
                        }

                        if (entity.gamebuff.find(oxorany("snake.entity.prefab")) != std::string::npos && var::esp::world::snake) {
                            std::string item_string = oxorany("Snake");
                            Vector3 itemPos = entity.player->GetBoneLocation(0);
                            DrawAnimalESP(itemPos, item_string, ImColor(255, 255, 255), ImColor(0, 255, 0));
                        }

                        if (entity.gamebuff.find(oxorany("shark.prefab")) != std::string::npos && var::esp::world::shark) {
                            std::string item_string = oxorany("Shark");
                            Vector3 itemPos = entity.player->GetBoneLocation(0);
                            DrawAnimalESP(itemPos, item_string, ImColor(255, 255, 255), ImColor(74, 72, 71));
                        }

                        if (entity.gamebuff.find(oxorany("wolf.prefab")) != std::string::npos && var::esp::world::wolf) {
                            std::string item_string = oxorany("Wolf");
                            Vector3 itemPos = entity.player->GetBoneLocation(0);
                            DrawAnimalESP(itemPos, item_string, ImColor(255, 255, 255), ImColor(138, 138, 138));
                        }

                        if (entity.gamebuff.find(oxorany("ridablehorse")) != std::string::npos && var::esp::world::horse) {
                            std::string item_string = oxorany("Horse");
                            Vector3 itemPos = entity.player->GetBoneLocation(0);
                            DrawAnimalESP(itemPos, item_string, ImColor(255, 255, 255), ImColor(41, 23, 9));
                        }

                        if (entity.gamebuff.find(oxorany("chicken.prefab")) != std::string::npos && var::esp::world::chicken) {
                            std::string item_string = oxorany("Chicken");
                            Vector3 itemPos = entity.player->GetBoneLocation(0);
                            DrawAnimalESP(itemPos, item_string, ImColor(255, 255, 255), ImColor(191, 187, 184));
                        }
                    }

                    if (var::esp::deployables::enable) {
                        if (entity.gamebuff.find(oxorany("sleepingbag")) != std::string::npos && var::esp::deployables::sleepingbag) {
                            std::string item_string = oxorany("Sleeping Bag");
                            Vector3 itemPos = entity.player->GetBoneLocation(0);
                            DrawAnimalESP(itemPos, item_string, ImColor(255, 255, 255), ImColor(146, 146, 146));
                        }

                        if (entity.gamebuff.find(oxorany("bed_deployed")) != std::string::npos && var::esp::deployables::bed_deployed) {
                            std::string item_string = oxorany("Bed");
                            Vector3 itemPos = entity.player->GetBoneLocation(0);
                            DrawAnimalESP(itemPos, item_string, ImColor(255, 255, 255), ImColor(186, 186, 186));
                        }

                        if (entity.gamebuff.find(oxorany("cupboard.tool")) != std::string::npos && var::esp::deployables::toolcupboard) {
                            std::string item_string = oxorany("Tool Cupboard");
                            Vector3 itemPos = entity.player->GetBoneLocation(0);
                            DrawAnimalESP(itemPos, item_string, ImColor(255, 255, 255), ImColor(138, 85, 67));
                        }

                        if (entity.gamebuff.find(oxorany("autoturret")) != std::string::npos && var::esp::deployables::turret) {
                            std::string item_string = oxorany("Turret");
                            Vector3 itemPos = entity.player->GetBoneLocation(0);
                            DrawAnimalESP(itemPos, item_string, ImColor(255, 255, 255), ImColor(107, 255, 211));
                        }

                        if (entity.gamebuff.find(oxorany("guntrap")) != std::string::npos && var::esp::deployables::guntrap) {
                            std::string item_string = oxorany("Shotgun Trap");
                            Vector3 itemPos = entity.player->GetBoneLocation(0);
                            DrawAnimalESP(itemPos, item_string, ImColor(255, 255, 255), ImColor(255, 255, 128));
                        }

                        if (entity.gamebuff.find(oxorany("flameturret.deployed.prefab")) != std::string::npos && var::esp::deployables::fireturret) {
                            std::string item_string = oxorany("Fire Turret");
                            Vector3 itemPos = entity.player->GetBoneLocation(0);
                            DrawAnimalESP(itemPos, item_string, ImColor(255, 255, 255), ImColor(255, 255, 128));
                        }

                        if (entity.gamebuff.find(oxorany("sam_site_turret_deployed.prefab")) != std::string::npos && var::esp::deployables::samsite) {
                            std::string item_string = oxorany("Samsite");
                            Vector3 itemPos = entity.player->GetBoneLocation(0);
                            DrawAnimalESP(itemPos, item_string, ImColor(255, 255, 255), ImColor(255, 255, 128));
                        }
                    }
                }
            }
        }
    }

    //void OtherShit() {
    //    bool hasPlayers = false;

    //    for (auto& player : esp->player_list) {

    //        hasPlayers = true;

    //        if (var::menu::Watermark) {
    //            auto watermark = oxorany("firox base | @gworld. | firox.lol Rust | developer");
    //            auto fps_text = std::format(" | FPS: {:.1f}", ImGui::GetIO().Framerate);
    //            std::string full_text = std::string(watermark) + fps_text;

    //            auto full_text_size = ImGui::CalcTextSize(full_text.c_str());

    //            auto padding_x = 6;
    //            auto padding_y = 4;
    //            auto yOffset = 5;

    //            ImVec2 rect_min(6, 4 + yOffset);
    //            ImVec2 rect_max(6 + full_text_size.x + (padding_x * 2), 4 + yOffset + full_text_size.y + (padding_y * 2));

    //            DrawList->AddRectFilled(rect_min, ImVec2(rect_max.x, rect_min.y + 2), ImColor(178, 102, 255));
    //            DrawList->AddRectFilled(ImVec2(rect_min.x, rect_min.y + 2), rect_max, ImColor(15, 15, 15));

    //            ImVec2 text_pos(rect_min.x + padding_x, rect_min.y + padding_y);
    //            DrawList->AddText(text_pos, ImColor(255, 255, 255, 255), full_text.c_str());
    //        }
    //        if (var::aim::fovvisible) RenderFOVCircle(var::aim::fovsize, var::aim::fovvisible);
    //        if (var::esp::Radar) DrawRadar();
    //        if (var::esp::fovarrows) RenderFOVArrows();
    //    }
    //}

    //void ChamLoop() {
    //    auto localPlayer = data->LocalPlayer;
    //    auto heldItem = localPlayer->GetHeldItem();
    //    if (!heldItem) return;

    //    static std::string previousWeaponName;
    //        std::string weaponName = heldItem->get_short_name();
    //        if (weaponName != previousWeaponName) {
    //            previousWeaponName = weaponName;

    //            auto baseProjectileChams = tRead<uintptr_t>(heldItem->Address() + O::Projectile::HeldItem);
    //            auto viewModel = tRead<uintptr_t>(baseProjectileChams + O::BaseProjectile::viewModel);
    //            auto baseViewModel = tRead<uintptr_t>(viewModel + O::BaseProjectile::viewModels::baseviewModel);

    //            tWrite<bool>(baseViewModel + 0x40, false);
    //            auto viewModelObject = ReadChain<uintptr_t>(baseViewModel, { 0x10, 0x30 });

    //            std::vector<uintptr_t> renderWeapon, renderHand;
    //            GetComponentsInChildren(viewModelObject, renderWeapon, renderHand);

    //            if (var::esp::chams::HandChams) {
    //                for (auto renderer : renderHand) {
    //                    ProcessSkinnedMeshRenderer(renderer, 142124);
    //                }
    //            }

    //            if (var::esp::chams::WeaponChams) {
    //                for (auto renderer : renderWeapon) {
    //                    ProcessSkinnedMeshRenderer(renderer, 142124);
    //                }
    //            }
    //        }

    //        if (var::esp::chams::PlayerChams) {
    //            auto playermodel = tRead<uintptr_t>(reinterpret_cast<uintptr_t>(esp->player_list[0].ptr) + O::BasePlayer::PlayerModel);
    //            auto skinnedMultiMesh = tRead<uint64_t>(playermodel + O::PlayerModel::SkinnedMultiMesh);
    //            auto skinnedRenderersList = tRead<uint64_t>(skinnedMultiMesh + O::PlayerModel::RendererList);
    //            auto skinnedList = tRead<uint64_t>(skinnedRenderersList + O::entitycount);
    //            int materialsCount = tRead<int>(skinnedRenderersList + O::entityarray);

    //            for (uint32_t idx = 0; idx < materialsCount; idx++) {
    //                const auto renderEntry = tRead<uintptr_t>(skinnedList + 0x20 + (idx * 0x8));
    //                if (!renderEntry) continue;

    //                const auto unityObject = tRead<uintptr_t>(renderEntry + O::entityarray);
    //                if (!unityObject) continue;

    //                const auto matList = tRead<dynamic_array>(unityObject + O::PlayerModel::MaterialList);
    //                if (matList.sz < 1 || matList.sz > 8) continue;

    //                for (uint32_t matIdx = 0; matIdx < matList.sz; matIdx++) {
    //                    unsigned int newChamType = 1;
    //                    switch (var::esp::chams::chamtype) {
    //                    case 0: newChamType = 0; break;
    //                    case 1: newChamType = 291414; break; // Glow_Redmaterial 
    //                    case 2: newChamType = 291342; break; // Glow_Greenmaterial
    //                    case 3: newChamType = 2131320; break;
    //                    case 4: newChamType = 142124; break;
    //                    default: newChamType = 0; break;
    //                    }
    //                    tWrite<unsigned int>(matList.base + (matIdx * 0x4), newChamType);
    //            }
    //        }
    //     }
    //  }
    //

    //void ExploitLoop() {
    //    if (var::esp::exploits::enable) {
    //    static bool needs_reset_recoil = false;
    //    static uintptr_t lastWeaponPtr = 0;
    //    bool hasPlayers = false;

    //    for (const auto& player : esp->player_list) {
    //        hasPlayers = true;

    //        auto heldItem = data->LocalPlayer->GetHeldItem();
    //        auto BaseProjectileInstaEoka = tRead<uintptr_t>(heldItem->Address() + O::Projectile::HeldItem);
    //        auto Movement = tRead<uintptr_t>(reinterpret_cast<uintptr_t>(data->LocalPlayer) + O::BasePlayer::PlayerMovement);
    //        auto mov_flag = tRead<uintptr_t>(reinterpret_cast<uintptr_t>(data->LocalPlayer) + O::BasePlayer::ModelState);
    //        auto RecoilProperties = tRead<uintptr_t>(BaseProjectileInstaEoka + O::BaseProjectile::recoil);
    //        auto newRecoilOverride = tRead<uintptr_t>(RecoilProperties + O::RecoilProperties::new_recoil);
    //        auto viewModel = tRead<uintptr_t>(BaseProjectileInstaEoka + O::BaseProjectile::viewModel);
    //        auto view_model_instance = tRead<uintptr_t>(viewModel + O::BaseProjectile::viewModels::baseviewModel);
    //        auto view_model_object = ReadChain<uintptr_t>(view_model_instance, { 0x10, 0x30 });
    //        const std::string& WeaponNameReal = heldItem->get_short_name();

    //        if (var::esp::exploits::spiderman && data->LocalPlayer) {
    //            tWrite<float>(Movement + 0x108, 0.0f);
    //        }

    //        if (var::esp::exploits::shoot_in_air) {
    //            tWrite<float>(Movement + 0x54, 9999.0f);
    //            tWrite<int>(mov_flag + 0x30, 4);
    //            tWrite<float>(Movement + 0x100, 1.0f);
    //            tWrite<float>(Movement + 0x108, 1.0f);
    //        }

    //        if (var::esp::exploits::instant_eoka) {
    //            if (!tRead<bool>(BaseProjectileInstaEoka + O::BaseProjectileExploits::DidShitTick)) {
    //                tWrite<bool>(BaseProjectileInstaEoka + O::BaseProjectileExploits::DidShitTick, true);
    //            }
    //            auto currentFlintStrikeWeapon = tRead<float>(BaseProjectileInstaEoka + O::BaseProjectileExploits::FlintStrikeWeapon);
    //            if (currentFlintStrikeWeapon != var::esp::exploits::eoka_chance) {
    //                tWrite<float>(BaseProjectileInstaEoka + O::BaseProjectileExploits::FlintStrikeWeapon, var::esp::exploits::eoka_chance);
    //            }
    //        }

    //        if (var::esp::exploits::fake_admin || var::esp::exploits::thirdperson) {
    //            auto playerFlags = tRead<int>(reinterpret_cast<uintptr_t>(data->LocalPlayer) + O::BasePlayer::playerFlags);
    //            if (playerFlags != 64 && playerFlags != 16) {
    //                if (var::esp::exploits::fake_admin && playerFlags != 4) {
    //                    tWrite<int>(reinterpret_cast<uintptr_t>(data->LocalPlayer) + O::BasePlayer::playerFlags, 4);
    //                }
    //                if (var::esp::exploits::thirdperson && playerFlags != 1024) {
    //                    tWrite<int>(reinterpret_cast<uintptr_t>(data->LocalPlayer) + O::BasePlayer::playerFlags, 1024);
    //                }
    //            }
    //        }

    //        if (var::esp::exploits::force_burst && !tRead<bool>(BaseProjectileInstaEoka + O::BaseProjectile::isBurstWeapon)) {
    //            tWrite<bool>(BaseProjectileInstaEoka + O::BaseProjectile::isBurstWeapon, true);
    //        }

    //        if (var::esp::exploits::fast_burst) {
    //            tWrite<float>(BaseProjectileInstaEoka + 0x2D4, 1.2f);
    //            tWrite<float>(BaseProjectileInstaEoka + O::BaseProjectile::internalBurstFireRateScale, 0.4f);
    //        }

    //        if (var::esp::exploits::no_bob) {
    //            auto ViewmodelBob = tRead<uintptr_t>(view_model_instance + O::BaseProjectile::viewModels::baseViewModel::ViewmodelBob);
    //            if (tRead<float>(ViewmodelBob + O::BaseProjectile::viewModels::baseViewModel::viewmodelBob::bobAmountRun) != 0.0f) {
    //                tWrite<float>(ViewmodelBob + O::BaseProjectile::viewModels::baseViewModel::viewmodelBob::bobAmountRun, 0.0f);
    //            }
    //            if (tRead<float>(ViewmodelBob + O::BaseProjectile::viewModels::baseViewModel::viewmodelBob::bobAmountWalk) != 0.0f) {
    //                tWrite<float>(ViewmodelBob + O::BaseProjectile::viewModels::baseViewModel::viewmodelBob::bobAmountWalk, 0.0f);
    //            }
    //        }

    //        if (var::esp::exploits::norecoil_x != 100.0f || var::esp::exploits::norecoil_y != 100.0f) {
    //            needs_reset_recoil = true;
    //            auto key = getWeaponKey(WeaponNameReal);
    //            auto it = recoilMap.find(key);

    //            if (it != recoilMap.end() && Valid(RecoilProperties) && Valid(newRecoilOverride)) {
    //                if (heldItem->Address() != lastWeaponPtr) {
    //                    auto lastIt = recoilMap.find(key);
    //                    if (lastIt != recoilMap.end()) {
    //                        tWrite<float>(RecoilProperties + O::RecoilProperties::recoilYawMin, lastIt->second.RecoilProperties[0]);
    //                        tWrite<float>(RecoilProperties + O::RecoilProperties::recoilYawMax, lastIt->second.RecoilProperties[1]);
    //                        tWrite<float>(RecoilProperties + O::RecoilProperties::recoilPitchMin, lastIt->second.RecoilProperties[2]);
    //                        tWrite<float>(RecoilProperties + O::RecoilProperties::recoilPitchMax, lastIt->second.RecoilProperties[3]);
    //                        tWrite<float>(newRecoilOverride + O::RecoilProperties::recoilYawMin, lastIt->second.NewRecoilOverride[0]);
    //                        tWrite<float>(newRecoilOverride + O::RecoilProperties::recoilYawMax, lastIt->second.NewRecoilOverride[1]);
    //                        tWrite<float>(newRecoilOverride + O::RecoilProperties::recoilPitchMin, lastIt->second.NewRecoilOverride[2]);
    //                        tWrite<float>(newRecoilOverride + O::RecoilProperties::recoilPitchMax, lastIt->second.NewRecoilOverride[3]);
    //                    }
    //                    lastWeaponPtr = heldItem->Address();
    //                }

    //                float scaleFactorX = std::clamp(var::esp::exploits::norecoil_x, 0.0f, 100.0f) / 100.0f;
    //                float scaleFactorY = std::clamp(var::esp::exploits::norecoil_y, 0.0f, 100.0f) / 100.0f;

    //                tWrite<float>(RecoilProperties + O::RecoilProperties::recoilYawMin, it->second.RecoilProperties[0] * scaleFactorX);
    //                tWrite<float>(RecoilProperties + O::RecoilProperties::recoilYawMax, it->second.RecoilProperties[1] * scaleFactorX);
    //                tWrite<float>(RecoilProperties + O::RecoilProperties::recoilPitchMin, it->second.RecoilProperties[2] * scaleFactorY);
    //                tWrite<float>(RecoilProperties + O::RecoilProperties::recoilPitchMax, it->second.RecoilProperties[3] * scaleFactorY);
    //                tWrite<float>(newRecoilOverride + O::RecoilProperties::recoilYawMin, it->second.NewRecoilOverride[0] * scaleFactorX);
    //                tWrite<float>(newRecoilOverride + O::RecoilProperties::recoilYawMax, it->second.NewRecoilOverride[1] * scaleFactorX);
    //                tWrite<float>(newRecoilOverride + O::RecoilProperties::recoilPitchMin, it->second.NewRecoilOverride[2] * scaleFactorY);
    //                tWrite<float>(newRecoilOverride + O::RecoilProperties::recoilPitchMax, it->second.NewRecoilOverride[3] * scaleFactorY);
    //            }
    //        }
    //        else if (needs_reset_recoil) {
    //            auto key = getWeaponKey(WeaponNameReal);
    //            auto it = recoilMap.find(key);
    //            if (it != recoilMap.end() && Valid(RecoilProperties) && Valid(newRecoilOverride)) {
    //                tWrite<float>(RecoilProperties + O::RecoilProperties::recoilYawMin, it->second.RecoilProperties[0]);
    //                tWrite<float>(RecoilProperties + O::RecoilProperties::recoilYawMax, it->second.RecoilProperties[1]);
    //                tWrite<float>(RecoilProperties + O::RecoilProperties::recoilPitchMin, it->second.RecoilProperties[2]);
    //                tWrite<float>(RecoilProperties + O::RecoilProperties::recoilPitchMax, it->second.RecoilProperties[3]);
    //                tWrite<float>(newRecoilOverride + O::RecoilProperties::recoilYawMin, it->second.NewRecoilOverride[0]);
    //                tWrite<float>(newRecoilOverride + O::RecoilProperties::recoilYawMax, it->second.NewRecoilOverride[1]);
    //                tWrite<float>(newRecoilOverride + O::RecoilProperties::recoilPitchMin, it->second.NewRecoilOverride[2]);
    //                tWrite<float>(newRecoilOverride + O::RecoilProperties::recoilPitchMax, it->second.NewRecoilOverride[3]);
    //                needs_reset_recoil = false;
    //            }
    //        }

    //        if (var::esp::exploits::spread_value != 100.0f) {
    //            needs_reset_recoil = true;
    //            float scale = var::esp::exploits::spread_value / 100.0f;
    //            tWrite<float>(BaseProjectileInstaEoka + O::BaseProjectile::sightAimConeScale, scale);
    //            tWrite<float>(BaseProjectileInstaEoka + O::BaseProjectile::hipAimConeScale, scale);
    //        }
    //        else if (needs_reset_recoil) {
    //            tWrite<float>(BaseProjectileInstaEoka + O::BaseProjectile::sightAimConeScale, 1.0f);
    //            tWrite<float>(BaseProjectileInstaEoka + O::BaseProjectile::hipAimConeScale, 1.0f);
    //            needs_reset_recoil = false;
    //        }

    //        if (var::esp::exploits::printgun) {
    //            std::cout << oxorany("Weapon Name: ") << WeaponNameReal << '\n';
    //        }

    //        if (var::esp::exploits::always_automatic && !tRead<bool>(BaseProjectileInstaEoka + O::BaseProjectile::automatic)) {
    //            tWrite<bool>(BaseProjectileInstaEoka + O::BaseProjectile::automatic, true);
    //        }
    //    }
    //   }
    //}
}


namespace Core {
void EntityUpdate() {
    while (true) {

        esp->data_temp.clear();
        auto Objects = BaseNetworkable::GetObjects();
        if (!Objects) continue;

        data->LocalPlayer = Objects->GetData()->GetLocalPlayer();

        size_t size = Objects->GetSize();
        esp->data_temp.reserve(size);
        for (size_t i = 0; i < size; ++i) {
            auto pEntity = Objects->GetData()->GetEntity(i);
            if (!pEntity) continue;

            auto EntityObject = pEntity->GetEntityObject();
            if (!EntityObject) continue;

            auto GameObject = EntityObject->GetGameObject();
            if (!GameObject) continue;

            std::string GameBuff = GameObject->GetGameBuff();
            if (GameBuff.empty()) continue;

            esp->data_temp.emplace_back(ESP::ESPData{ GameBuff, (BasePlayer*)pEntity, pEntity, EntityObject });
        }

        esp->data_list = std::move(esp->data_temp);
        esp->player_temp.clear();

        for (const auto& entity : esp->data_list) {
            if (entity.gamebuff.find(oxorany("player.prefab")) != std::string::npos ||
                (entity.gamebuff.find(oxorany("scientistnpc")) != std::string::npos && var::esp::show_scientists)) {
                esp->m_player.ptr = entity.player;
                esp->player_temp.emplace_back(esp->m_player);
            }
        }

        esp->player_list = esp->player_temp.empty() ? std::vector{ esp->m_player } : std::move(esp->player_temp);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
    void Run() {
        EntityUpdate();
    }
}