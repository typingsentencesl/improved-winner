#include "Core/includes.h"

bool FullInit = false;
int main()
{
	drv::Init(true);
	drv::procid = drv::FindProcess(oxorany(L"RustClient.exe"));
	game->Pid = drv::procid;
	if (game->Pid == 0) {
		Log::Error(oxorany("Failed to find the game process: "), true);
		__fastfail(11);
	}
	drv::Base = drv::GetBase();
	game->GameAssembly = drv::get_module(L"GameAssembly.dll");
	game->UnityPlayer = drv::get_module(L"UnityPlayer.dll");

	std::stringstream ss;
	ss << oxorany("GameAssembly Address: 0x") << std::hex << game->GameAssembly;
	Log::LogSuccess(ss.str());
	ss.str(oxorany(""));
	ss << oxorany("UnityPlayer Address: 0x") << std::hex << game->UnityPlayer;
	Log::LogSuccess(ss.str());
	ss.str(oxorany(""));
	ss << oxorany("Rust Address: 0x") << std::hex << drv::Base;
	Log::LogSuccess(ss.str());
	ss.str(oxorany(""));

	Sleep(1000);

	if (!FullInit)
	{
		Overlay::CreateOverlay();
		Overlay::DirectX();
		FullInit = true;
	}
	std::thread(Core::EntityUpdate).detach();

	StartRendering();


	Sleep(10000);
}