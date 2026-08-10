union DWSPLITTER {
	unsigned long long full;
	struct { unsigned long low; unsigned long high; };
};

#define LODWORD(x) ((DWSPLITTER*)&x)->low
#define HIDWORD(x) ((uint32_t)(((uint64_t)(x) >> 32) & 0xFFFFFFFF))
#define COERCE_FLOAT(value) (*reinterpret_cast<float*>(&value))
#define __int128 uint64_t
#define _OWORD uint32_t
#define _BYTE  uint8_t
#define _WORD  uint16_t
#define _DWORD uint32_t
#define _QWORD uint64_t
#define BYTEn(x, n)   (*((_BYTE*)&(x)+n))
#define LAST_IND(x,part_type)    (sizeof(x)/sizeof(part_type) - 1)
#define HIGH_IND(x,part_type)  LAST_IND(x,part_type)
#define HIBYTE(x) BYTEn(x,HIGH_IND(x,_BYTE))
#define TEST_BITD(value, bit) ((value) & (1 << (bit)))
#define BIT_TEST(value, bit) ((value) & (1 << ((bit) & 0x1F)))

inline uintptr_t Il2cppGetHandle(int32_t ObjectHandleID) {
	uint64_t rdi_1 = ((uint64_t)(ObjectHandleID >> 3));
	uint64_t rcx_1 = ((uint64_t)((ObjectHandleID & 7) - 1));
	uintptr_t ObjectArray = read<uintptr_t>((rcx_1 * 0x28) + (game->GameAssembly + O::ghandle + 0x8)) + (rdi_1 << 3);
	if (read<uint8_t>((rcx_1 * 0x28) + (game->GameAssembly + O::ghandle + 0x14)) > 1)
	{
		return read<uintptr_t>(ObjectArray);
	}
	else
	{
		uint32_t eax = read<uint32_t>(ObjectArray);
		eax = ~eax;
		return eax;
	}
}

// Updated to build 24614784
// BaseNetworkable client_entities / visiblePlayerList
// rol 31, add 0x8F7F58E3, rol 31, xor 0x64DE867F (x2 dwords)
uint64_t BaseNetworkableKey(uint64_t address)
{
	if (!read<uint8_t>(address + 0x10))
		return 0;
	uint64_t rax_val = read<uint64_t>(address + 0x18);
	uint32_t* rdx = (uint32_t*)&rax_val;
	uint32_t r8d = 2;
	uint32_t eax, ecx;

	do
	{
		ecx = *(uint32_t*)(rdx);
		eax = *(uint32_t*)(rdx);
		rdx = (uint32_t*)((uint8_t*)rdx + 0x4);
		eax = ecx;
		ecx = ecx << 0x1F;
		eax = eax >> 0x1;
		ecx = ecx | eax;
		ecx = ecx + 0x8F7F58E3;
		eax = ecx;
		ecx = ecx << 0x1F;
		eax = eax >> 0x1;
		ecx = ecx | eax;
		ecx = ecx ^ 0x64DE867F;
		*((uint32_t*)rdx - 1) = ecx;
		--r8d;
	} while (r8d);
	return Il2cppGetHandle(rax_val);
}

// DecryptList is BaseNetworkable entity_list
// add 0x374D4FA, rol 27, xor 0x5DD1D7B5, sub 0x10D77418 (x2 dwords)
uint64_t DecryptList(uint64_t address)
{
	if (!read<uint8_t>(address + 0x10))
		return 0;
	uint64_t rax_val = read<uint64_t>(address + 0x18);
	uint32_t* rdx = (uint32_t*)&rax_val;
	uint32_t r8d = 2;
	uint32_t eax, ecx;

	do
	{
		ecx = *(uint32_t*)(rdx);
		eax = *(uint32_t*)(rdx);
		rdx = (uint32_t*)((uint8_t*)rdx + 0x4);
		ecx = ecx + 0x374D4FA;
		eax = ecx;
		ecx = ecx << 0x1B;
		eax = eax >> 0x5;
		ecx = ecx | eax;
		ecx = ecx ^ 0x5DD1D7B5;
		ecx = ecx - 0x10D77418;
		*((uint32_t*)rdx - 1) = ecx;
		--r8d;
	} while (r8d);
	return Il2cppGetHandle(rax_val);
}

// PlayerInventory wrapper
// rol 12, add 0xD06D4709, rol 21 (x2 dwords)
uint64_t DecryptPlayerInventory(uint64_t address)
{
	if (!read<uint8_t>(address + 0x10))
		return 0;
	uint64_t rax_val = read<uint64_t>(address + 0x18);
	uint32_t* rdx = (uint32_t*)&rax_val;
	uint32_t r8d = 2;
	uint32_t eax, ecx;

	do
	{
		ecx = *(uint32_t*)(rdx);
		eax = *(uint32_t*)(rdx);
		rdx = (uint32_t*)((uint8_t*)rdx + 0x4);
		eax = ecx;
		ecx = ecx << 0xC;
		eax = eax >> 0x14;
		ecx = ecx | eax;
		ecx = ecx + 0xD06D4709;
		eax = ecx;
		ecx = ecx << 0x15;
		eax = eax >> 0xB;
		ecx = ecx | eax;
		*((uint32_t*)rdx - 1) = ecx;
		--r8d;
	} while (r8d);
	return Il2cppGetHandle(rax_val);
}

// PlayerEyes wrapper
// add 0x45CC7567, rol 8, add 0x3443CA46 (x2 dwords)
uint64_t DecryptPlayerEyes(uint64_t address)
{
	if (!read<uint8_t>(address + 0x10))
		return 0;
	uint64_t rax_val = read<uint64_t>(address + 0x18);
	uint32_t* rdx = (uint32_t*)&rax_val;
	uint32_t r8d = 2;
	uint32_t eax, ecx;

	do
	{
		ecx = *(uint32_t*)(rdx);
		eax = *(uint32_t*)(rdx);
		rdx = (uint32_t*)((uint8_t*)rdx + 0x4);
		ecx = ecx + 0x45CC7567;
		eax = ecx;
		ecx = ecx << 0x8;
		eax = eax >> 0x18;
		ecx = ecx | eax;
		ecx = ecx + 0x3443CA46;
		*((uint32_t*)rdx - 1) = ecx;
		--r8d;
	} while (r8d);
	return Il2cppGetHandle(rax_val);
}

// clActiveItem — a1 is the encrypted u64 value (not a wrapper pointer)
// xor 0xC7F486D7, add 0x21AB371, rol 11, add 0x70077F50 (x2 dwords)
// returns decrypted value (not a handle)
std::uintptr_t DecryptClActiveItem(std::uintptr_t a1)
{
	std::uintptr_t rax = a1;
	uint32_t* rdx = (uint32_t*)&rax;
	uint32_t r8d = 2;
	uint32_t eax, ecx;

	do
	{
		ecx = *(uint32_t*)(rdx);
		eax = *(uint32_t*)(rdx);
		rdx = (uint32_t*)((uint8_t*)rdx + 0x4);
		ecx = ecx ^ 0xC7F486D7;
		ecx = ecx + 0x21AB371;
		eax = ecx;
		ecx = ecx << 0xB;
		eax = eax >> 0x15;
		ecx = ecx | eax;
		ecx = ecx + 0x70077F50;
		*((uint32_t*)rdx - 1) = ecx;
		--r8d;
	} while (r8d);
	return rax;
}

// encrypt_value — STALE, commented out (no matching routine in build 24614784)
// std::uint32_t encrypt_value(std::uint32_t input) {
// 	std::uint32_t temp1 = (input << 31) | (input >> 1);
// 	std::uint32_t temp2 = temp1 + 0x267EEF74;
// 	std::uint32_t temp3 = temp2 ^ 0x3829AFFB;
// 	std::uint32_t result = temp3 + 0x38001986;
// 	return result;
// }

// FOV encrypt — reversed from new decrypt_fov:
//   decrypt: rol 27, xor 0xBF1B783D, rol 30
//   encrypt: ror 30, xor 0xBF1B783D, ror 27
uint64_t EncryptFov(float value)
{
	uint32_t v = *(uint32_t*)&value;
	// reverse of: rol 30
	v = (v >> 30) | (v << 2);
	// reverse of: xor
	v ^= 0xBF1B783Du;
	// reverse of: rol 27
	v = (v >> 27) | (v << 5);
	return static_cast<uint64_t>(v);
}
