#define IMGUI_DEFINE_MATH_OPERATORS
#include "..\ImGui\imgui.h"
#include "..\ImGui\imgui_internal.h"
#include "..\ImGui\imgui_impl_dx9.h"
#include "..\ImGui\imgui_impl_win32.h"
#include "..\ImGui\custom.hpp"
#include "..\ImGui\images.hpp"
#include "..\ImGui\hotkey.hpp"
#include "..\ImGui\fonts.hpp"
#include <dwmapi.h>
#include <d3d9.h>
#include "d3dx9tex.h"
#include <tchar.h>
#include "map"
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "d3dx9.lib")
#include "..\..\lazy.hpp"
float color[3] = {
    (float)((var::menu::AccentColor >> IM_COL32_R_SHIFT) & 0xFF) / 255.0f,
    (float)((var::menu::AccentColor >> IM_COL32_G_SHIFT) & 0xFF) / 255.0f,
    (float)((var::menu::AccentColor >> IM_COL32_B_SHIFT) & 0xFF) / 255.0f
};

bool OnDraw = false;
D3DPRESENT_PARAMETERS p_params = { NULL };
IDirect3DDevice9Ex* p_device = NULL;
IDirect3D9Ex* p_object = NULL;
ImFont* menu_ico_font, * default_font;

LPDIRECT3DTEXTURE9 LoadTextureFromMemory(LPDIRECT3DDEVICE9 pDevice, const std::vector<uint8_t>& data) {
    if (data.empty()) return nullptr;
    LPDIRECT3DTEXTURE9 texture = nullptr;
    D3DXCreateTextureFromFileInMemory(pDevice, data.data(), data.size(), &texture);
    return texture;
}

HWND GetProcessWindow(uint32_t pid) {
    std::pair<HWND, uint32_t> params = { 0, pid };
    EnumWindows([](HWND hwnd, LPARAM lparam) -> BOOL {
        auto pparams = (std::pair<HWND, uint32_t>*)(lparam);
        uint32_t processid = 0;
        if (GetWindowThreadProcessId(hwnd, reinterpret_cast<LPDWORD>(&processid)) && processid == pparams->second) {
            SetLastError((uint32_t)-1);
            pparams->first = hwnd;
            return FALSE;
        }
        return TRUE;
        }, (LPARAM)&params);

    return (!GetLastError() == -1 || !params.first) ? 0 : params.first;
}

LPDIRECT3DTEXTURE9 fatassblueblob_texture = nullptr;

inline void RenderMenu() {
    if (GetAsyncKeyState(VK_INSERT) & 1) Backend::showMenu = !Backend::showMenu;

    if (!Backend::showMenu) return;

    ImGui::SetNextWindowSize({ 620, 500 });
    ImGui::Begin(oxorany("firox shit base"), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar);
    {
        ImGui::Text(oxorany("firox shit base | @gworld. | firox.lol | Developer Build"));
        ImGui::Separator();

        if (ImGui::Button(oxorany("Aim"), ImVec2(151, 25))) Backend::current_tab = 0;
        ImGui::SameLine(0, 0);
        if (ImGui::Button(oxorany("Visuals"), ImVec2(151, 25))) Backend::current_tab = 1;
        ImGui::SameLine(0, 0);
        if (ImGui::Button(oxorany("Misc"), ImVec2(151, 25))) Backend::current_tab = 2;
        ImGui::SameLine(0, 0);
        if (ImGui::Button(oxorany("Settings"), ImVec2(151, 25))) Backend::current_tab = 3;

        const float window_width = ImGui::GetContentRegionAvailWidth() - ImGui::GetStyle().WindowPadding.x;
        const float window_height = ImGui::GetContentRegionAvail().y - ImGui::GetStyle().WindowPadding.y;

        switch (Backend::current_tab)
        {
        case 0:
            imgui_custom->begin_child(oxorany("Aimbot Configuration"), ImVec2(window_width * 0.5f, window_height));
            {
                ImGui::Checkbox(oxorany("Enable Aimbot"), &var::aim::Enable);
                //ImGui::Checkbox(oxorany("Enable Silentaim"), &var::aim::silentaim);
                //ImGui::Checkbox(oxorany("Prediction"), &var::aim::Prediction);
                ImGui::Checkbox(oxorany("FOV Circle"), &var::aim::fovvisible);
                ImGui::Checkbox(oxorany("Target Line"), &var::aim::targetline);
                ImGui::Checkbox(oxorany("Aimbot Smoothing"), &var::aim::Smoothness);
            }
            imgui_custom->end_child();

            ImGui::SameLine();

            imgui_custom->begin_child(oxorany("Extra Aimbot Configuration"), ImVec2(window_width * 0.5f, window_height));
            {
                ImGui::SliderFloat(oxorany("FOV Size"), &var::aim::fovsize, 5, 500, "%.f");
                ImGui::SliderFloat(oxorany("FOV Segments"), &var::aim::fovsegments, 0, 50, "%.f");
                ImGui::SliderFloat(oxorany("FOV Thickness"), &var::aim::fovthickness, 0.1f, 10.f, "%.1f");
                if (var::aim::Smoothness) {
                    ImGui::SliderFloat(oxorany("Smoothness"), &var::aim::Smooth, 0.0f, 3.0f, "%.1f");
                }
                ImGui::ColorEdit3(oxorany("FOV Color"), var::esp::FovColor, ImGuiColorEditFlags_NoInputs);
                ImGui::Spacing();
                ImGui::Text(oxorany("Aimbot Keybind"));
                hotkey_button(var::aim::Key, ChangeKeyAim, keystatus);
                //ImGui::Text(oxorany("Gun Modifications"));
                //ImGui::Spacing();
                //ImGui::SliderFloat(oxorany("Recoil X"), &var::esp::exploits::norecoil_x, 0.0f, 100.f, "%.1f");
                //ImGui::SliderFloat(oxorany("Recoil Y"), &var::esp::exploits::norecoil_y, 0.0f, 100.f, "%.1f");
                //ImGui::SliderFloat(oxorany("Spread"), &var::esp::exploits::spread_value, 0.0f, 100.f, "%.1f");
            }
            imgui_custom->end_child();
            break;

        case 1:
            imgui_custom->begin_child(oxorany("Visual Configuration"), ImVec2(window_width * 0.5f, window_height * 0.5f));
            {
                if (ImGui::Checkbox(oxorany("Box"), &var::esp::Box) && var::esp::Box)
                    var::esp::CornerBox = false;
                if (ImGui::Checkbox(oxorany("Cornered Box"), &var::esp::CornerBox) && var::esp::CornerBox)
                    var::esp::Box = false;

                ImGui::Checkbox(oxorany("Skeleton (Fps Rape)"), &var::esp::Skeleton);
                ImGui::Checkbox(oxorany("Head Circle (Fps Rape)"), &var::esp::HeadCircle);
                ImGui::Checkbox(oxorany("Username"), &var::esp::Name);
                ImGui::Checkbox(oxorany("Held Weapon"), &var::esp::Weapon);
                ImGui::Checkbox(oxorany("Distance"), &var::esp::Distance);
                ImGui::Checkbox(oxorany("Indication Arrows"), &var::esp::fovarrows);
                ImGui::Checkbox(oxorany("Radar"), &var::esp::Radar);
                //ImGui::Spacing();
                //ImGui::Checkbox(oxorany("Player Chams"), &var::esp::chams::PlayerChams);
                //ImGui::Checkbox(oxorany("Hand Chams"), &var::esp::chams::HandChams);
                //ImGui::Checkbox(oxorany("Weapon Chams"), &var::esp::chams::WeaponChams);
            }
            imgui_custom->end_child();

            ImGui::SameLine();

            imgui_custom->begin_child(oxorany("Extra Visual Configuration"), ImVec2(window_width * 0.5f, window_height * 0.5f));
            {
                ImGui::Checkbox(oxorany("Outlined"), &var::esp::Outlined);
                if (var::esp::HeadCircle) {
                    ImGui::SliderFloat(oxorany("Head Circle Size"), &var::esp::HeadCircleSize, 1.0f, 50.0f, "%.1f");
                }
                ImGui::ColorEdit3(oxorany("Visuals Color"), var::esp::VisualsColor, ImGuiColorEditFlags_NoInputs);
                ImGui::ColorEdit3(oxorany("Sleeper Color"), var::esp::SleeperColor, ImGuiColorEditFlags_NoInputs);
                ImGui::ColorEdit3(oxorany("Wounded Color"), var::esp::WoundedColor, ImGuiColorEditFlags_NoInputs);
                ImGui::ColorEdit3(oxorany("Scientist Color"), var::esp::ScientistColor, ImGuiColorEditFlags_NoInputs);
                ImGui::Spacing();
                ImGui::SliderFloat(oxorany("Radar X"), &var::esp::RadarX, 0.f, 1920, "%.1f");
                ImGui::SliderFloat(oxorany("Radar Y"), &var::esp::RadarY, 0.f, 1080, "%.1f");
                ImGui::Checkbox(oxorany("Usernames on Radar"), &var::esp::usernames_radar);
                ImGui::Checkbox(oxorany("Distance on Radar"), &var::esp::distance_radar);
            }
            imgui_custom->end_child();

            ImGui::Dummy(ImVec2(0, 1.0f));

            imgui_custom->begin_child(oxorany("Deployables Configuration"), ImVec2(window_width * 0.5f, window_height * 0.5f));
            {
                ImGui::Checkbox(oxorany("Enable"), &var::esp::deployables::enable);
                ImGui::Checkbox(oxorany("Patrol Helicopter"), &var::esp::deployables::patrolheli);
                ImGui::Checkbox(oxorany("Sleeping Bag"), &var::esp::deployables::sleepingbag);
                ImGui::Checkbox(oxorany("Bed"), &var::esp::deployables::bed_deployed);
                ImGui::Checkbox(oxorany("Tool Cupboard"), &var::esp::deployables::toolcupboard);
                ImGui::Checkbox(oxorany("Turret"), &var::esp::deployables::turret);
                ImGui::Checkbox(oxorany("Fire Turret"), &var::esp::deployables::fireturret);
                ImGui::Checkbox(oxorany("Shotgun Trap"), &var::esp::deployables::guntrap);
                ImGui::Checkbox(oxorany("Samsite"), &var::esp::deployables::samsite);
            }
            imgui_custom->end_child();

            ImGui::SameLine();

            imgui_custom->begin_child(oxorany("World Configuration"), ImVec2(window_width * 0.5f, window_height * 0.5f));
            {
                ImGui::Checkbox(oxorany("Enable"), &var::esp::world::enable);
                ImGui::Checkbox(oxorany("Boar"), &var::esp::world::boar);
                ImGui::Checkbox(oxorany("Bear"), &var::esp::world::bear);
                ImGui::Checkbox(oxorany("Wolf"), &var::esp::world::wolf);
                ImGui::Checkbox(oxorany("Horse"), &var::esp::world::horse);
                ImGui::Checkbox(oxorany("Chicken"), &var::esp::world::chicken);
                ImGui::Checkbox(oxorany("Shark"), &var::esp::world::shark);
                ImGui::Checkbox(oxorany("Deer"), &var::esp::world::deer);
                ImGui::Checkbox(oxorany("Snake"), &var::esp::world::snake);
            }
            imgui_custom->end_child();

            ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x + 5.0f, ImGui::GetWindowPos().y + 25.0f));
            ImGui::SetNextWindowSize({ window_width * 0.5f, window_height });
            ImGui::Begin(oxorany("Preview"), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
            {
                ImVec2 preview_center = ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x * 0.5f, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y * 0.5f);
                ImGui::Text(oxorany("                                          Esp Preview"));
                ImGui::Separator();

                if (fatassblueblob_texture) {
                    ImVec2 texture_size = ImVec2(160, 300);
                    ImVec2 texture_pos = ImVec2(preview_center.x - texture_size.x * 0.5f, preview_center.y - texture_size.y * 0.5f);
                    ImGui::GetWindowDrawList()->AddImage(fatassblueblob_texture, texture_pos, ImVec2(texture_pos.x + texture_size.x, texture_pos.y + texture_size.y));
                }

                ImU32 visualColor = ImGui::GetColorU32(ImVec4(var::esp::VisualsColor[0], var::esp::VisualsColor[1], var::esp::VisualsColor[2], 1.0f));
                ImDrawList* drawList = ImGui::GetWindowDrawList();
                ImVec2 box_min = ImVec2(preview_center.x - 80, preview_center.y - 150);
                ImVec2 box_max = ImVec2(preview_center.x + 80, preview_center.y + 150);

                if (var::esp::Box) {
                    drawList->AddRect(box_min, box_max, visualColor);
                }

                if (var::esp::CornerBox) {
                    float corner_length = 20.0f;
                    // Top-left
                    drawList->AddLine(ImVec2(box_min.x, box_min.y), ImVec2(box_min.x + corner_length, box_min.y), visualColor);
                    drawList->AddLine(ImVec2(box_min.x, box_min.y), ImVec2(box_min.x, box_min.y + corner_length), visualColor);
                    // Top-right
                    drawList->AddLine(ImVec2(box_max.x, box_min.y), ImVec2(box_max.x - corner_length, box_min.y), visualColor);
                    drawList->AddLine(ImVec2(box_max.x, box_min.y), ImVec2(box_max.x, box_min.y + corner_length), visualColor);
                    // Bottom-left
                    drawList->AddLine(ImVec2(box_min.x, box_max.y), ImVec2(box_min.x + corner_length, box_max.y), visualColor);
                    drawList->AddLine(ImVec2(box_min.x, box_max.y), ImVec2(box_min.x, box_max.y - corner_length), visualColor);
                    // Bottom-right
                    drawList->AddLine(ImVec2(box_max.x, box_max.y), ImVec2(box_max.x - corner_length, box_max.y), visualColor);
                    drawList->AddLine(ImVec2(box_max.x, box_max.y), ImVec2(box_max.x, box_max.y - corner_length), visualColor);
                }

                if (var::esp::Name) {
                    const char* userName = oxorany("firox shit base user | @gworld. | firox.lol");
                    ImVec2 textSize = ImGui::CalcTextSize(userName);
                    ImVec2 text_pos = ImVec2(preview_center.x - textSize.x * 0.5f, preview_center.y - 170);
                    drawList->AddText(text_pos, visualColor, userName);
                }

                if (var::esp::Weapon) {
                    const char* weaponName = oxorany("Assault Rifle");
                    ImVec2 textSize = ImGui::CalcTextSize(weaponName);
                    ImVec2 text_pos = ImVec2(preview_center.x - textSize.x * 0.5f, preview_center.y + 155);
                    drawList->AddText(text_pos, visualColor, weaponName);
                }

                if (var::esp::HeadCircle) {
                    ImVec2 circle_center = ImVec2(preview_center.x, preview_center.y - 150);
                    drawList->AddCircle(circle_center, 15.0f, visualColor, 0, 2.0f);
                }
            }
            ImGui::End();
            break;

        case 2:
            imgui_custom->begin_child(oxorany("Misc Configuration"), ImVec2(window_width * 0.5f, window_height));
            {
                ImGui::Checkbox(oxorany("Watermark"), &var::menu::Watermark);
                ImGui::Checkbox(oxorany("Ignore Teamates"), &var::esp::TeamCheck);
                ImGui::Checkbox(oxorany("Ignore Sleepers"), &var::esp::SkipSleepers);
                ImGui::Checkbox(oxorany("Ignore Wounded"), &var::esp::SkipKnocked);
                ImGui::Checkbox(oxorany("Ignore Scientists"), &var::esp::show_scientists);
            }
            imgui_custom->end_child();

            ImGui::SameLine();

            imgui_custom->begin_child(oxorany("Exploit Configuration"), ImVec2(window_width * 0.5f, window_height));
            {
                ImGui::Checkbox(oxorany("Enable Exploits"), &var::esp::exploits::enable);

                if (var::esp::exploits::enable) {
                    ImGui::Checkbox(oxorany("Spiderman"), &var::esp::exploits::spiderman);
                    //ImGui::Checkbox(oxorany("Shoot in Air"), &var::esp::exploits::shoot_in_air);
                    //ImGui::Checkbox(oxorany("No View Bob"), &var::esp::exploits::no_bob);
                    //ImGui::Checkbox(oxorany("No Weapon Lower"), &var::esp::exploits::no_lower);
                    ImGui::Checkbox(oxorany("Fake Admin"), &var::esp::exploits::fake_admin);
                    //ImGui::Checkbox(oxorany("Burst"), &var::esp::exploits::fast_burst);
                    //ImGui::Checkbox(oxorany("Instant Eoka"), &var::esp::exploits::instant_eoka);
                    //ImGui::Checkbox(oxorany("Always Automatic"), &var::esp::exploits::always_automatic);
                    //ImGui::Checkbox(oxorany("Print Gun"), &var::esp::exploits::printgun);

                    if (var::esp::exploits::instant_eoka) {
                        ImGui::SliderFloat(oxorany("Eoka Miss Chance"), &var::esp::exploits::eoka_chance, 0.0f, 100.f);
                    }

                    ImGui::Checkbox(oxorany("Third Person"), &var::esp::exploits::thirdperson);
                }
            }
            imgui_custom->end_child();
            break;

        case 3:
            imgui_custom->begin_child(oxorany("Settings Configuration"), ImVec2(0, 0));
            {
                if (ImGui::Button(oxorany("Unload"), ImVec2(100, 25))) exit(0);
            }
            imgui_custom->end_child();
            break;
        }
    }
    ImGui::End();
}

WPARAM StartRendering() {
    MSG msg = {};
    RECT OldRect = { 0 };
    RECT Rect = { 0 };
    LARGE_INTEGER frequency, lastTime, currentTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastTime);

    if (!fatassblueblob_texture) {
        fatassblueblob_texture = LoadTextureFromMemory(p_device, astolfo_esp_preview);
    }

    while (true) {
        SetWindowPos(var::menu::hwnd, GetWindow(GetForegroundWindow(), GW_HWNDPREV), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
        GameWindow = GetProcessWindow(game->Pid);
        bool OnDraw = (GetForegroundWindow() == GameWindow);

        if (GameWindow && GetClientRect(GameWindow, &Rect)) {
            if (Rect.right != OldRect.right || Rect.bottom != OldRect.bottom) {
                OldRect = Rect;
                Backend::width = Rect.right;
                Backend::height = Rect.bottom;
                p_params.BackBufferWidth = Backend::width;
                p_params.BackBufferHeight = Backend::height;
                SetWindowPos(var::menu::hwnd, nullptr, 0, 0, Backend::width, Backend::height, SWP_NOREDRAW | SWP_NOZORDER);
                p_device->Reset(&p_params);
            }
        }

        QueryPerformanceCounter(&currentTime);
        ImGuiIO& io = ImGui::GetIO();
        io.DeltaTime = static_cast<float>(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
        lastTime = currentTime;

        POINT cursor_pos;
        GetCursorPos(&cursor_pos);
        io.MousePos = ImVec2(cursor_pos.x, cursor_pos.y);
        io.MouseDown[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;

        if (GetAsyncKeyState(VK_INSERT) & 1)
            Backend::showMenu ^= true;

        SetWindowDisplayAffinity(var::menu::hwnd, var::settings::AntiRecord ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (OnDraw) {
            RenderLoop::Run();
            SetThreadPriority(RenderLoop::Run, THREAD_PRIORITY_HIGHEST);
			RenderLoop::AimbotLoop();
            SetThreadPriority(RenderLoop::AimbotLoop, THREAD_PRIORITY_HIGHEST);
			RenderLoop::WorldEsp();
            SetThreadPriority(RenderLoop::WorldEsp, THREAD_PRIORITY_HIGHEST);
            RenderMenu();
        }

        ImGui::EndFrame();

        p_device->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
        if (SUCCEEDED(p_device->BeginScene())) {
            if (OnDraw) {
                ImGui::Render();
                ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            }
            p_device->EndScene();
        }

        HRESULT presentResult = p_device->Present(nullptr, nullptr, nullptr, nullptr);
        if (presentResult == D3DERR_DEVICELOST && p_device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
            ImGui_ImplDX9_InvalidateDeviceObjects();
            p_device->Reset(&p_params);
            ImGui_ImplDX9_CreateDeviceObjects();
        }
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    if (p_device) p_device->Release();
    if (p_object) p_object->Release();
    DestroyWindow(var::menu::hwnd);
    return msg.wParam;
}

bool amd = false;
bool nvidia = false;

namespace Overlay {
    typedef HWND(WINAPI* CreateWindowInBand)(_In_ DWORD dwExStyle, _In_opt_ ATOM atom, _In_opt_ LPCWSTR lpWindowName, _In_ DWORD dwStyle, _In_ int X, _In_ int Y, _In_ int nWidth, _In_ int nHeight, _In_opt_ HWND hWndParent, _In_opt_ HMENU hMenu, _In_opt_ HINSTANCE hInstance, _In_opt_ LPVOID lpParam, DWORD band);

    void CreateOverlay() {
        WNDCLASSEXA wcex = {
            sizeof(WNDCLASSEXA), 0, DefWindowProcA, 0, 0, nullptr,
            LoadIcon(nullptr, IDI_APPLICATION), LoadCursor(nullptr, IDC_ARROW),
            nullptr, nullptr, (oxorany("Chrome_WidgetWin_1")), LoadIcon(nullptr, IDI_APPLICATION)
        };

        RECT Rect;
        GetWindowRect(GetDesktopWindow(), &Rect);

        if (!RegisterClassExA(&wcex)) {
            return;
        }

        var::menu::hwnd = CreateWindowExA(
            NULL,
            oxorany("Chrome_WidgetWin_1"),
            oxorany("@gworld. - Discord"),
            WS_POPUP,
            Rect.left,
            Rect.top,
            Rect.right,
            Rect.bottom,
            NULL,
            NULL,
            wcex.hInstance,
            NULL
        );

        SetWindowLong(var::menu::hwnd, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_LAYERED);
        MARGINS margin = { -1 };
        DwmExtendFrameIntoClientArea(var::menu::hwnd, &margin);
        ShowWindow(var::menu::hwnd, SW_SHOW);
        SetWindowPos(var::menu::hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
        SetLayeredWindowAttributes(var::menu::hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
        UpdateWindow(var::menu::hwnd);
    }

    static bool updated = false;
    ImFont* icon_fnt = nullptr;

    HRESULT DirectX() {
        if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_object)))
            exit(3);

        ZeroMemory(&p_params, sizeof(p_params));
        p_params.Windowed = TRUE;
        p_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
        p_params.hDeviceWindow = var::menu::hwnd;
        p_params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
        p_params.BackBufferFormat = D3DFMT_A8R8G8B8;
        p_params.BackBufferWidth = Backend::width;
        p_params.BackBufferHeight = Backend::height;
        p_params.EnableAutoDepthStencil = TRUE;
        p_params.AutoDepthStencilFormat = D3DFMT_D16;
        p_params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

        if (FAILED(p_object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, var::menu::hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_params, 0, &p_device))) {
            p_object->Release();
            exit(1006);
        }

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        static ImWchar ranges[] = { 0x1, 0x1FFFF, 0 };

        static ImFontConfig cfg;
        if (!updated) {
            io.Fonts->AddFontFromMemoryTTF(UrBestFont, sizeof UrBestFont, 14.0f, &cfg, ranges);
            if (!default_font)
                default_font = io.Fonts->AddFontFromMemoryTTF(UrBestFont, sizeof UrBestFont, 14.0f, &cfg, ranges);
            io.IniFilename = 0;
            updated = true;
        }

        ImGui::StyleColorsDark();
        ImGuiStyle* style = &ImGui::GetStyle();
        style->Colors[ImGuiCol_WindowBg] = ImColor(10, 10, 10, 255);
        style->Colors[ImGuiCol_Border] = ImColor(45, 45, 45, 255);
        style->Colors[ImGuiCol_Separator] = ImColor(178, 102, 255);
        style->Colors[ImGuiCol_SeparatorHovered] = ImColor(178, 102, 255);
        style->Colors[ImGuiCol_SeparatorActive] = ImColor(178, 102, 255);
        style->Colors[ImGuiCol_ChildBg] = ImColor(15, 15, 15, 255);
        style->Colors[ImGuiCol_FrameBg] = ImColor(25, 25, 25, 255);
        style->Colors[ImGuiCol_PopupBg] = ImColor(25, 25, 25, 255);
        style->Colors[ImGuiCol_Header] = ImColor(0, 0, 0, 0);
        style->Colors[ImGuiCol_HeaderHovered] = ImColor(0, 0, 0, 0);
        style->Colors[ImGuiCol_HeaderActive] = ImColor(0, 0, 0, 0);
        style->Colors[ImGuiCol_Button] = ImColor(25, 25, 25, 255);
        style->Colors[ImGuiCol_ButtonActive] = ImColor(25, 25, 25, 255);
        style->Colors[ImGuiCol_ButtonHovered] = ImColor(25, 25, 25, 255);
        style->Colors[ImGuiCol_CheckMark] = ImColor(178, 102, 255);
        style->Colors[ImGuiCol_SliderGrab] = ImColor(178, 102, 255);
        style->WindowTitleAlign = { 0.5f, 0.5f };
        style->WindowRounding = 0.0f;
        style->ChildRounding = 0.0f;
        style->FrameRounding = 0.0f;
        style->WindowBorderSize = 1.0f;
        style->FrameBorderSize = 1.0f;
        style->ChildBorderSize = 1.0f;
        style->AntiAliasedFill = false;
        style->AntiAliasedLines = false;
        style->Alpha = 1.0f;

        ImGui_ImplWin32_Init(var::menu::hwnd);
        ImGui_ImplDX9_Init(p_device);
        p_object->Release();
        return S_OK;
    }
}
