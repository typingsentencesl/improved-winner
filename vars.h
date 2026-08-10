#pragma once
#include "Core/ImGui/imgui.h"
#include "Windows.h"

namespace var
{
	namespace settings
	{
		bool AntiRecord = false;
	}
	namespace aim
	{
		bool Enable = true;
		bool Smoothness = false;
		bool SkipSleepers = false;
		bool Prediction = false;
		auto Key = VK_XBUTTON2;
		bool silentaim = false;
		bool targetline = true;
		bool WaitingForKey = false;
		float Smooth = 1.f;
		float fovsize = 90.0f;
		bool fovvisible = true;
		float fovthickness = 1.0f;
		float fovsegments = 100;
		bool norecoil = false;
	}

	namespace esp
	{
		bool SkipSleepers = true;
		bool Outlined = false;
		bool SkipKnocked = true;
		bool TeamCheck = false;
		bool CornerBox = false;
		bool Name = true;
		bool Box = true;
		bool show_scientists = false;
		bool Weapon = false;
		bool fovarrows = false;
		bool Distance = false;
		bool Skeleton = false;
		bool HeadCircle = false;
		float HeadCircleSize = 10.0f;
		bool DrawBones = false;

		bool usernames_radar = false;
		bool distance_radar = false;
		float RadarX = 0.0f;
		float RadarY = 0.0f;
		bool Radar = false;

		float VisualsColor[3] = { 1.0f, 0.2f, 1.0f };
		float ScientistColor[3] = { 1.0f, 0.2f, 1.0f };
		float WoundedColor[3] = { 1.0f, 0.2f, 1.0f };
		float SleeperColor[3] = { 1.0f, 0.2f, 1.0f };
		float FovColor[3] = { 1.0f, 0.2f, 1.0f };

		namespace chams
		{
			bool PlayerChams = false;
			bool HandChams = false;
			bool WeaponChams = false;
			int chamtype = 1;
		}

		namespace world
		{
			bool enable = false;

			bool bear = false;
			bool bearimage = false;

			bool deer = false;
			bool deerimage = false;

			bool snake = false;
			bool snakeimage = false;

			bool chicken = false;
			bool chickenimage = false;

			bool boar = false;
			bool boarimage = false;

			bool shark = false;
			bool sharkimage = false;

			bool wolf = false;
			bool wolfimage = false;

			bool horse = false;
			bool horseimage = false;
		}
		
		namespace deployables
		{
			bool enable = false;
			bool sleepingbag = false;
			bool bed_deployed = false;
			bool toolcupboard = false;
			bool turret = false;
			bool guntrap = false;
			bool patrolheli = false;
			bool beartrap = false;
			bool fireturret = false;
			bool samsite = false;
			bool landmine = false;
			bool vendingmachine = false;
			bool generator = false;
			bool battery = false;
		}

		namespace exploits {
			bool enable = false;
			bool spiderman = false;
			bool shoot_in_air = false;
			bool no_bob = false;
			bool fake_admin = false;
			bool printgun = false;
			bool fast_burst = false;
			bool instant_eoka = false;
			bool always_automatic = false;
			bool force_burst = false;
			bool no_lower = false;
			bool thirdperson = false;
			float eoka_chance = 0.0f;
			float norecoil_x = 100.0f;
			float norecoil_y = 100.0;
			float spread_value = 100.0f;
		}

	}

	namespace menu
	{
		ImU32 AccentColor = ImColor(255, 120, 255, 255);
		HWND hwnd;
		bool Watermark = true;
	}
}

