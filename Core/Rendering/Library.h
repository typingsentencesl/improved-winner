#pragma once

//namespace library
//{
//	HMODULE user32 = nullptr;
//	HMODULE win32u = nullptr;
//	bool is_initialized = false;
//
//	bool init()
//	{
//		if (is_initialized) return true;
//
//		user32 = GetModuleHandleA(skCrypt("user32.dll"));
//		if (!user32) return false;
//
//		win32u = GetModuleHandleA(skCrypt("win32u.dll"));
//		if (!win32u) return false;
//
//		is_initialized = true;
//		return true;
//	}
//}
