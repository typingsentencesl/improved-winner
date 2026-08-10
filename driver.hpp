#pragma once
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <string>
#include <random>
#include <vector>
#include <winternl.h>

#define HYPERBOLIC_TWIST(x) ((((x) & 0xCCDDBFAE) >> 6) | (((x) & 0x22134557) << 3) | (((x) & 0x99AABB33) >> 5))
#define POLYMORPHIC_RIGHT(x, r) (((x) >> ((r) % 13 + 1)) | ((x) << (32 - ((r) % 13 + 1))))
#define POLYMORPHIC_LEFT(x, r) (((x) << ((r) % 11 + 1)) | ((x) >> (32 - ((r) % 11 + 1))))
#define METAMORPHIC_VALUE(x) ((POLYMORPHIC_LEFT(x, 13) ^ 0xE9D8C7B6) + 0x78563412 - POLYMORPHIC_RIGHT(~x, 7))
#define FEISTEL_NETWORK(x) (((x) * 0x8E7D6C5B) + ((x) >> 15) + ((x) << 9))

#define ENTROPY_GENERATOR(x) (FEISTEL_NETWORK(METAMORPHIC_VALUE(HYPERBOLIC_TWIST(x))) % 0x7FFAC3DB)

#define MAGIC_PRIME1 0xF62A3D7
#define MAGIC_PRIME2 0xB792A1F

#define ReadWrite_code CTL_CODE(FILE_DEVICE_UNKNOWN, ENTROPY_GENERATOR(MAGIC_PRIME1) ^ MAGIC_PRIME1, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define BaseAddress_code CTL_CODE(FILE_DEVICE_UNKNOWN, ENTROPY_GENERATOR(MAGIC_PRIME2) ^ MAGIC_PRIME2, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

typedef struct _RWOp
{
	INT32 ProcessID;
	ULONGLONG Address, Buffer, Size;
	BOOLEAN Write;
	BOOLEAN CR3;
} RWOp, * pRWOp;

typedef struct _BaseAddrQUR
{
	INT32 ProcID;
	ULONGLONG* TargetPtr;
} BaseAddrQUR, * pBaseAddrQUR;

namespace drv {
	bool eac = false;
	INT32 procid;
	auto Base = 0;
	HANDLE DrvHandle;

	bool Init(bool EAC = false) {
		eac = EAC;
		DrvHandle = CreateFileA("\\\\.\\F68D23B1C", GENERIC_READ | GENERIC_WRITE, 0, 0, 3, 0x00000080, 0);
		if (!DrvHandle) {
			return false;
		}
		return true;
	}

	void ReadPhys(PVOID address, PVOID buffer, DWORD size)
	{
		_RWOp Arguments = { 0 };
		Arguments.Address = (ULONGLONG)address;
		Arguments.Buffer = (ULONGLONG)buffer;
		Arguments.Size = size;
		Arguments.ProcessID = procid;
		Arguments.Write = false;
		Arguments.CR3 = eac;

		DeviceIoControl(DrvHandle, ReadWrite_code, &Arguments, sizeof(Arguments), nullptr, NULL, NULL, NULL);
	}

	void WritePhys(PVOID address, PVOID buffer, DWORD size)
	{
		_RWOp Arguments = { 0 };
		Arguments.Address = (ULONGLONG)address;
		Arguments.Buffer = (ULONGLONG)buffer;
		Arguments.Size = size;
		Arguments.ProcessID = procid;
		Arguments.Write = true;
		Arguments.CR3 = eac;

		DeviceIoControl(DrvHandle, ReadWrite_code, &Arguments, sizeof(Arguments), nullptr, NULL, NULL, NULL);
	}
	uintptr_t GetBase()
	{
		uintptr_t image_address = { NULL };
		_BaseAddrQUR Arguments = { NULL };

		Arguments.ProcID = procid;
		Arguments.TargetPtr = (ULONGLONG*)&image_address;
		DeviceIoControl(DrvHandle, BaseAddress_code, &Arguments, sizeof(Arguments), nullptr, NULL, NULL, NULL);

		return image_address;
	}
	inline INT32 FindProcess(LPCTSTR process_name)
	{

		PROCESSENTRY32 pt;
		HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		pt.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hsnap, &pt)) {
			do {
				if (!lstrcmpi(pt.szExeFile, process_name))
				{
					CloseHandle(hsnap);
					procid = pt.th32ProcessID;
					return pt.th32ProcessID;
				}
			} while (Process32Next(hsnap, &pt));
		}

		CloseHandle(hsnap);
		return procid;
	}

	inline std::uintptr_t get_module(const wchar_t* name)
	{
		const auto handle = OpenProcess(PROCESS_QUERY_INFORMATION, 0, procid);
		auto current = 0ull;
		auto mbi = MEMORY_BASIC_INFORMATION();
		while (VirtualQueryEx(handle, reinterpret_cast<void*>(current), &mbi, sizeof(MEMORY_BASIC_INFORMATION)))
		{
			if (mbi.Type == MEM_MAPPED || mbi.Type == MEM_IMAGE)
			{
				const auto buffer = malloc(1024);
				auto bytes = std::size_t();
				const static auto ntdll = GetModuleHandleA(("ntdll"));
				const static auto nt_query_virtual_memory_fn =
					reinterpret_cast<NTSTATUS(__stdcall*)(HANDLE, void*, std::int32_t, void*, std::size_t, std::size_t*)> (
						GetProcAddress(ntdll, ("NtQueryVirtualMemory")));

				if (nt_query_virtual_memory_fn(handle, mbi.BaseAddress, 2, buffer, 1024, &bytes) != 0 ||
					!wcsstr(static_cast<UNICODE_STRING*>(buffer)->Buffer, name) ||
					wcsstr(static_cast<UNICODE_STRING*>(buffer)->Buffer, (L".mui")))
				{
					free(buffer);
					goto skip;
				}
				free(buffer);
				CloseHandle(handle);

				return reinterpret_cast<std::uintptr_t>(mbi.BaseAddress);
			}
		skip:
			current = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
		}
		CloseHandle(handle);
		return 0ull;
	}

}

template <typename T>
inline T read(uint64_t address)
{
	T buffer{ };
	drv::ReadPhys((PVOID)address, &buffer, sizeof(T));
	return buffer;
}

template <typename T>
inline T write(uint64_t address, T buffer)
{
	drv::WritePhys((PVOID)address, &buffer, sizeof(T));
	return buffer;
}

struct UnityStrBuff {
	char buffer[128];
};

template <typename T>
inline T tRead(uint64_t address) {
	T buffer{};
	drv::ReadPhys((PVOID)address, &buffer, sizeof(T));
	return buffer;
}

template<typename T>
T tWrite(uint64_t address, T buffer) {
	drv::WritePhys((PVOID)address, &buffer, sizeof(T));
	return buffer;
}

template<typename Type>
inline Type ReadChain(const std::uint64_t& Address, std::vector<std::uint64_t> Offsets)
{
	std::uint64_t Value = Address;
	for (int i = 0; i < Offsets.size() - 1; i++)
		Value = tRead<std::uint64_t>(Value + Offsets[i]);
	return tRead<Type>(Value + Offsets.back());
}

template <typename T>
inline T readchain(uint64_t address, std::vector<uint64_t> chain)
{
	uint64_t cur_read = address;
	for (size_t i = 0; i < chain.size() - 1; ++i)
	{
		cur_read = read<uint64_t>(cur_read + chain[i]);
		if (!cur_read)
			return T();
	}
	return read<T>(cur_read + chain.back());
}

std::string readstring(uint64_t Address)
{
	std::unique_ptr<char[]> buffer(new char[64]);
	drv::ReadPhys((PVOID)Address, buffer.get(), 64);
	return buffer.get();
}

std::string read_wstr(uintptr_t address)
{
	wchar_t buffer[1024];
	drv::ReadPhys(reinterpret_cast<PVOID>(address), buffer, sizeof(buffer));
	buffer[1023] = L'\0';

	std::wstring wstr(buffer);
	for (auto& ch : wstr)
		if (ch < 32 || ch > 126)
			ch = L'?';

	return std::string(wstr.begin(), wstr.end());
}

std::string ReadChar(uint64_t addr) {
	std::string str = read<UnityStrBuff>(addr).buffer;
	if (!str.empty())
		return str;
	return "31";
}