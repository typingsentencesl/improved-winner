
#include <Windows.h>
#include <iostream>
#include <tchar.h>
#include <dwmapi.h>
#include <chrono>
#include <vector>
#include "vars.h"
#include ".\Protect\oxorany_include.h"
#include <TlHelp32.h>
#include <thread>
#include <immintrin.h>  // SSE, SSE2, SSE3, SSSE3, SSE4.x, AVX  oth..
#include <xmmintrin.h>  
#include <emmintrin.h> 
#include <smmintrin.h> 
#include <tmmintrin.h> 
#include <stdint.h> 
#include <cstring> 
#include <cmath> 
#include <array>

#include "ImGui/DirectX/Include/d3d9.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx9.h"
#include "ImGui/imgui_impl_win32.h"

#pragma comment(lib, "d3d9.lib")

using namespace std::literals::chrono_literals;

#pragma comment(lib, "dwmapi.lib")

HWND CheatWindow = NULL;
HWND GameWindow = NULL;

struct Game {

    DWORD       Pid;
    DWORD       TempPid;

    uintptr_t GameAssembly = 0;
    uintptr_t UnityPlayer = 0;
};
Game* game = new Game;


#include "globals.h"

#include "Utils/Logger.h"
#include "Utils/Math.h"

#include "Protect/skCrypt.h"

#include "driver.hpp"


#include "Game/offsets.h"
#include "Game/decryptors.h"

#include "Game/classes.h"

#include "Core.h"

#include "Rendering/Bytes.h"
#include "Rendering/Library.h"
#include "Rendering/Render.h"