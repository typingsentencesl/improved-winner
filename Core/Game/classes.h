#include "memory.h"

#define FIELD(type, name, addr) \
    type name() const { \
        return read<type>(reinterpret_cast<uintptr_t>(this) + addr); \
    } \
  void name(type value) { \
        write<type>(reinterpret_cast<uintptr_t>(this) + addr, value); \
    }


struct Vector4 {
	float x, y, z, w;
};

inline const double ToRad(double degree)
{
	double pi = 3.14159265359;
	return (degree * (pi / 180));
}

inline Vector4 ToQuat(Vector3 Euler) {
	float c1 = cos(ToRad(Euler.x) / 2);
	float s1 = sin(ToRad(Euler.x) / 2);
	float c2 = cos(ToRad(Euler.y) / 2);
	float s2 = sin(ToRad(Euler.y) / 2);
	float c3 = cos(ToRad(Euler.z) / 2);
	float s3 = sin(ToRad(Euler.z) / 2);
	float c1c2 = c1 * c2;
	float s1s2 = s1 * s2;
	float c1s2 = c1 * s2;
	float s1c2 = s1 * c2;
	Vector4 Quat = {
		s1c2 * c3 + c1s2 * s3,
		c1s2 * c3 - s1c2 * s3,
		c1c2 * s3 - s1s2 * c3,
		c1c2 * c3 + s1s2 * s3
	};
	return Quat;
}

enum class PlayerFlags : int
{
	Unused1 = 1,
	Unused2 = 2,
	IsAdmin = 4,
	ReceivingSnapshot = 8,
	Sleeping = 16,
	Spectating = 32,
	Wounded = 64,
	IsDeveloper = 128,
	Connected = 256,
	ThirdPersonViewmode = 1024,
	EyesViewmode = 2048,
	ChatMute = 4096,
	NoSprint = 8192,
	Aiming = 16384,
	DisplaySash = 32768,
	Relaxed = 65536,
	SafeZone = 131072,
	ServerFall = 262144,
	Workbench1 = 1048576,
	Workbench2 = 2097152,
	Workbench3 = 4194304,
};

enum BoneList : int
{
	pelvis = 0,
	l_hip = 1,
	l_knee = 2,
	l_foot = 3,
	l_toe = 4,
	l_ankle_scale = 5,
	penis = 6,
	GenitalCensor = 7,
	GenitalCensor_LOD0 = 8,
	Inner_LOD0 = 9,
	GenitalCensor_LOD1 = 11,
	GenitalCensor_LOD2 = 12,
	r_hip = 13,
	r_knee = 14,
	r_foot = 15,
	r_toe = 16,
	r_ankle_scale = 17,
	spine1 = 18,
	spine1_scale = 19,
	spine2 = 20,
	spine3 = 21,
	spine4 = 22,
	l_clavicle = 23,
	l_upperarm = 24,
	l_forearm = 25,
	l_hand = 26,
	l_index1 = 27,
	l_index2 = 28,
	l_index3 = 29,
	l_little1 = 30,
	l_little2 = 31,
	l_little3 = 32,
	l_middle1 = 33,
	l_middle2 = 34,
	l_middle3 = 35,
	l_prop = 36,
	l_ring1 = 37,
	l_ring2 = 38,
	l_ring3 = 39,
	l_thumb1 = 40,
	l_thumb2 = 41,
	l_thumb3 = 42,
	IKtarget_righthand_min = 43,
	IKtarget_righthand_max = 44,
	l_ulna = 45,
	neck = 46,
	head = 47,
	jaw = 48,
	eyeTranform = 49,
	l_eye = 50,
	l_Eyelid = 51,
	r_eye = 52,
	r_Eyelid = 53,
	r_clavicle = 54,
	r_upperarm = 55,
	r_forearm = 56,
	r_hand = 57,
	r_index1 = 58,
	r_index2 = 59,
	r_index3 = 60,
	r_little1 = 61,
	r_little2 = 62,
	r_little3 = 63,
	r_middle2 = 65,
	r_middle3 = 66,
	r_prop = 67,
	r_ring1 = 68,
	r_ring2 = 69,
	r_ring3 = 70,
	r_thumb1 = 71,
	r_thumb2 = 72,
	r_thumb3 = 73,
	IKtarget_lefthand_min = 74,
	IKtarget_lefthand_max = 75,
	r_ulna = 76,
	l_breast = 77,
	r_breast = 78,
	BoobCensor = 79,
	BreastCensor_LOD0 = 80,
	BreastCensor_LOD1 = 83,
	BreastCensor_LOD2 = 84
};


class PlayerEyes
{
private:
public:
	std::uint64_t Address()
	{
		return (std::uint64_t)(this);
	}

	void SetBodyRotation(Vector4 Value)
	{
		tWrite<Vector4>(this->Address() + O::Eyes::body_rotation, Value);
	}

};


struct UnityString
{
	char pad[0x10];
	int length;
	wchar_t buffer[128 + 1];


	std::string Get()
	{
		UnityString* t2 = this;
		if (!t2)
			return std::string();

		UnityString ustr = read<UnityString>((uint64_t)t2);
		if (!ustr.buffer)
			return std::string();

		std::wstring tmpe(ustr.buffer);
		if (tmpe.empty())
			return std::string();

		int utf8Size = WideCharToMultiByte(CP_UTF8, 0, tmpe.c_str(), -1, nullptr, 0, nullptr, nullptr);
		if (utf8Size == 0)
			return std::string();

		std::string utf8Result(utf8Size, '\0');
		WideCharToMultiByte(CP_UTF8, 0, tmpe.c_str(), -1, &utf8Result[0], utf8Size, nullptr, nullptr);

		utf8Result.pop_back();

		return utf8Result;
	}

};

class Camera
{
public:
	static Camera* GetMainCamera()
	{
		auto Class = ReadChain<uint64_t>(game->GameAssembly, { O::dwMainCamera, 0xB8 });
		auto CameraObj = read<uint64_t>(Class + O::CameraManager::CameraObject);
		return read<Camera*>(CameraObj + 0x10);
	}

	Matrix GetViewMatrix()
	{
		auto addr = read<Matrix>((uintptr_t)this + 0x2FC); // view_matrix — build 24614784
		return addr;
	}

	Vector2 WorldToScreen(Vector3& In)
	{
		Matrix m = this->GetViewMatrix().transpose();
		Vector3 translationVector = Vector3(m._41, m._42, m._43);
		Vector3 up = Vector3(m._21, m._22, m._23);
		Vector3 right = Vector3(m._11, m._12, m._13);

		float w = translationVector.dot_product(In) + m._44;

		if (w < 0.098f)
			return Vector2(0, 0);

		float y = up.dot_product(In) + m._24;
		float x = right.dot_product(In) + m._14;
		auto vOut = Vector2((Backend::width / 2) * (1.f + x / w), (Backend::height / 2) * (1.f - y / w));

		return vOut;
	}

	bool WorldToScreen(Vector3& vIn, Vector2& vOut)
	{
		Matrix m = this->GetViewMatrix().transpose();
		Vector3 translationVector = Vector3(m._41, m._42, m._43);
		Vector3 up = Vector3(m._21, m._22, m._23);
		Vector3 right = Vector3(m._11, m._12, m._13);

		float w = translationVector.dot_product(vIn) + m._44;

		if (w < 0.098f)
			return false;

		float y = up.dot_product(vIn) + m._24;
		float x = right.dot_product(vIn) + m._14;
		vOut = Vector2((Backend::width / 2) * (1.f + x / w), (Backend::height / 2) * (1.f - y / w));

		return true;
	}

	Vector3 GetCameraLocation()
	{
		return tRead<Vector3>((uintptr_t)this + 0x444); // Camera position — build 24614784
	}

};

class BasePlayer;
class BaseNetworkable
{
public:
	static BaseNetworkable* GetObjects()
	{
		auto get_klass = ReadChain<uint64_t>(game->GameAssembly, { O::dwBaseNetworkable ,0xB8 });
		if (get_klass == 0) return nullptr;
		//printf("class: %llx\n", get_klass);
		auto get_list = read<uint64_t>(get_klass + O::BN::List1);
		//printf("list: %llx\n", get_list);
		if (get_list == 0) return nullptr;

		auto decrypt_list = BaseNetworkableKey(get_list);
		//printf("decrypt_list: %llx\n", decrypt_list);
		if (decrypt_list == 0) return nullptr;

		auto get_ptr = read<uint64_t>(decrypt_list + O::BN::List2);
		//printf("get_ptr: %llx\n", get_ptr);
		if (get_ptr == 0) return nullptr;

		auto decrypt_ptr = DecryptList(get_ptr);
		//printf("decrypt_ptr: %llx\n", decrypt_ptr);
		if (decrypt_ptr == 0) return nullptr;

		auto get_object = read<BaseNetworkable*>(decrypt_ptr + O::BN::List3);
		//printf("get_object: %llx\n", get_object);
		if (get_object == 0) return nullptr;

		return get_object;

	}

	int32_t GetSize()
	{
		return read<int32_t>((uintptr_t)this + 0x18); // entitycount
	}
	BaseNetworkable* GetData()
	{
		auto entity = read<BaseNetworkable*>((uint64_t)this + 0x10); // entityarray
		if (!entity) return 0;

		return entity;
	}
	BaseNetworkable* GetEntity(int i)
	{
		auto entity = read<BaseNetworkable*>((uint64_t)this + (0x20 + (i * 8)));
		if (!entity) return 0;

		return entity;
	}
	BaseNetworkable* GetEntityObject()
	{
		auto EntityObject = read<BaseNetworkable*>((uint64_t)this + 0x10);
		if (!EntityObject) return nullptr;

		return EntityObject;
	}
	BaseNetworkable* GetGameObject()
	{
		auto GameObject = read<BaseNetworkable*>((uint64_t)this + 0x30);
		if (!GameObject) return nullptr;

		return GameObject;
	}
	std::string GetGameBuff()
	{
		uint64_t GameBuff = read<uint64_t>((uint64_t)this + 0x60);
		if (!GameBuff)
			return std::string();

		return ReadChar(GameBuff);
	}

	BasePlayer* GetLocalPlayer()
	{
		auto entity = read<BasePlayer*>((uint64_t)this + (0x20 + (0 * 8)));
		if (entity) return entity;

		int32_t count = this->GetSize();
		for (int32_t i = 0; i < count && i < 64; ++i) {
			auto candidate = this->GetEntity(i);
			if (!candidate) continue;
			auto obj = candidate->GetEntityObject();
			if (!obj) continue;
			auto game_obj = obj->GetGameObject();
			if (!game_obj) continue;
			std::string buff = game_obj->GetGameBuff();
			if (buff.find("LocalPlayer") != std::string::npos || buff.find("player.prefab") != std::string::npos) {
				return (BasePlayer*)candidate;
			}
		}
		return nullptr;
	}
};
class Transform
{
public:
	struct transform_access_read_only_t
	{
		uint64_t data;
	};

	struct transform_data_t
	{
		uint64_t array;
		uint64_t indices;
	};

	Vector3 GetBonePosition()
	{
		uintptr_t transform = (uintptr_t)this;
		if (!transform)
		{
			return { 0.f, 0.f, 0.f };
		}
		struct matrix34_t { BYTE _0[16]; BYTE _1[16]; BYTE _2[16]; };
		__m128 result{};

		const __m128 mulVec0 = { -2.000f, 2.000f, -2.000f, 0.000f };
		const __m128 mulVec1 = { 2.000, -2.000f, -2.000f, 0.000f };
		const __m128 mulVec2 = { -2.000f, -2.000f, 2.000f, 0.000f };

		transform_access_read_only_t transform_access_read_only = read<transform_access_read_only_t >(transform + 0x38);
		unsigned int index = read<unsigned int>(transform + 0x40);
		const auto transform_data = read<transform_data_t>(transform_access_read_only.data + 0x18);

		if (transform_data.array && transform_data.indices)
		{
			result = read<__m128>(transform_data.array + 0x30 * index);

			int transform_index = read<int>(transform_data.indices + 0x4 * index);

			int safe = 0;

			while (transform_index >= 0 && safe++ < 15)
			{
				matrix34_t matrix34 = read<matrix34_t>(transform_data.array + 0x30 * transform_index);
				__m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34._1), 0x00));    // xxxx
				__m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34._1), 0x55));    // yyyy
				__m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34._1), 0x8E));    // zwxy
				__m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34._1), 0xDB));    // wzyw
				__m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34._1), 0xAA));    // zzzz
				__m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*)(&matrix34._1), 0x71));    // yxwy
				__m128 tmp7 = _mm_mul_ps(*(__m128*)(&matrix34._2), result);

				result = _mm_add_ps(
					_mm_add_ps(
						_mm_add_ps(
							_mm_mul_ps(
								_mm_sub_ps(
									_mm_mul_ps(_mm_mul_ps(xxxx, mulVec1), zwxy),
									_mm_mul_ps(_mm_mul_ps(yyyy, mulVec2), wzyw)),
								_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
							_mm_mul_ps(
								_mm_sub_ps(
									_mm_mul_ps(_mm_mul_ps(zzzz, mulVec2), wzyw),
									_mm_mul_ps(_mm_mul_ps(xxxx, mulVec0), yxwy)),
								_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
						_mm_add_ps(
							_mm_mul_ps(
								_mm_sub_ps(
									_mm_mul_ps(_mm_mul_ps(yyyy, mulVec0), yxwy),
									_mm_mul_ps(_mm_mul_ps(zzzz, mulVec1), zwxy)),
								_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))),
							tmp7)), *(__m128*)(&matrix34._0));

				transform_index = read<int>(transform_data.indices + 0x4 * transform_index);
			}
		}

		return Vector3(result.m128_f32[0], result.m128_f32[1], result.m128_f32[2]);
	}
};

class BaseCombatEntity
{
public:

};
class Model
{
public:
	std::uint64_t Address()
	{
		return (std::uint64_t)(this);
	}

};

class BasePlayer : public BaseCombatEntity
{
public:
	struct bounds_t {
		float left, right, top, bottom;
		Vector3 world_position;
		bool is_empty() const {
			return (left == FLT_MAX || right == FLT_MIN || top == FLT_MAX || bottom == FLT_MIN);
		}
	};
	struct bone_t {
		Vector2 screen;
		int8_t     index;
		bool       on_screen;
		Vector3 world_position;

	};

	std::uint64_t Address()
	{
		return (std::uint64_t)(this);
	}

	Transform* GetTransform(int bone)
	{

		uint64_t player_model = (uintptr_t)this->model();
		if (!player_model)
		{
			return 0;
		}
		uint64_t boneTransforms = read<uint64_t>(player_model + O::Model::boneTransforms);
		if (!boneTransforms)
		{
			return 0;
		}
		DWORD64 BoneValue = read<DWORD64>((boneTransforms + (0x20 + (bone * 0x8))));
		if (!BoneValue)
		{
			return 0;
		}
		auto ret = read<Transform*>(BoneValue + 0x10);
		if (!ret)
		{
			return 0;
		}
		return ret;
	}

	Vector3 GetBoneLocation(int boneIndex) {
		Transform* trans = GetTransform(boneIndex);
		if (trans)
		{
			auto off = trans->GetBonePosition();//GetBone
			if (off.empty())
			{
				return Vector3(0, 0, 0);
			}
			return off;
		}

		return Vector3(0, 0, 0);
	}


	FIELD(UnityString*, _displayName, O::BasePlayer::_displayName);
	FIELD(Model*, model, O::BasePlayer::_Model);
	FIELD(PlayerFlags, playerFlags, O::BasePlayer::playerFlags);
	FIELD(uint64_t, teamID, O::BasePlayer::playerTeam);

	bool HasFlag(PlayerFlags flag)
	{
		return this->playerFlags() == flag;
	}

	BasePlayer* GetHeldItem() {
		auto player_inventory_encrypted = read<uint64_t>((uint64_t)this + O::BasePlayer::PlayerInventory);
		if (!player_inventory_encrypted)
			return 0;

		auto player_inventory = DecryptPlayerInventory(player_inventory_encrypted);
		if (!player_inventory)
			return 0;

		auto inventory_belt = read<uint64_t>(player_inventory + O::PlayerInventory::Belt);
		if (!inventory_belt)
			return 0;

		auto belt_contents_list = read<uint64_t>(inventory_belt + O::PlayerInventory::ItemList);
		if (!belt_contents_list)
			return 0;

		auto belt_contents = read<uint64_t>(belt_contents_list + 0x10);
		if (!belt_contents)
			return 0;

		auto active_item_id = read<uint64_t>((uint64_t)this + O::PlayerInventory::ClActiveItem);
		if (!active_item_id)
			return 0;

		auto decrypted_item_id = DecryptClActiveItem(active_item_id);
		if (!decrypted_item_id)
			return 0;

		for (int i = 0; i < 6; ++i) {
			auto current_item = read<uint64_t>(belt_contents + 0x20 + (i * 8));
			if (!current_item)
				continue;

			auto current_item_id = read<uint64_t>(current_item + O::PlayerInventory::ItemId);
			if (!current_item_id)
				continue;

			if (decrypted_item_id == current_item_id) {
				return (BasePlayer*)current_item;
			}
		}

		return 0;
	}

	std::string get_item_name() {
		if (!(uintptr_t)this) return "";
		uintptr_t unk = tRead<uintptr_t>((uintptr_t)this + O::PlayerInventory::ItemDefinition);

		uintptr_t ItemDisplayName = tRead< uintptr_t>(unk + O::PlayerInventory::ItemDisplayName);

		uintptr_t ItemDisplayEnglish = tRead< uintptr_t>(ItemDisplayName + O::PlayerInventory::ItemDisplayEnglish);

		return read_wstr(ItemDisplayEnglish + 0x14);
	}

	std::string get_short_name() {
		if (!(uintptr_t)this) return "None";
		uintptr_t unk = tRead<uintptr_t>((uintptr_t)this + O::PlayerInventory::ItemDefinition);

		uintptr_t ItemDisplayName = tRead< uintptr_t>(unk + O::PlayerInventory::ShortName);

		return read_wstr(ItemDisplayName + 0x14);
	}

	PlayerEyes* GetEyes()
	{
		auto PlayerEyes_encrypted = tRead<uint64_t>(this->Address() + O::BasePlayer::PlayerEyes);
		auto eyes = DecryptPlayerEyes(PlayerEyes_encrypted);
		return (PlayerEyes*)eyes;
	}

	Vector3 GetInputOffset()
	{
		auto input = tRead<uintptr_t>(this->Address() + O::BasePlayer::input);

		return tRead<Vector3>(input + 0x44);
	}


	void SetInputOffset(Vector3 value)
	{
		auto input = tRead<uintptr_t>(this->Address() + O::BasePlayer::input);
		tWrite<Vector3>(input + 0x44, value);
	}

	bool is_weapon() {
		static const std::vector<std::string> weaponitems = {
			(std::string)("rifle"),
			(std::string)("pistol"),
			(std::string)("lmg"),
			(std::string)("shotgun"),
			(std::string)("smg"),
			(std::string)("bow")
		};
		auto ws = this->get_short_name();
		std::string ItemName(ws.begin(), ws.end());
		for (const auto& item : weaponitems) {
			if (ItemName.find(item) != std::string::npos) return true;
		}
		return false;
	}

	bounds_t GetBounds(float expand) {
		bounds_t bounds = { FLT_MAX, FLT_MIN, FLT_MAX, FLT_MIN };
		std::array<bone_t, 10> bones = { {
			{Vector2{}, 17, false, Vector3{}}, {Vector2{}, 18, false, Vector3{}},
			{Vector2{}, 23, false, Vector3{}}, {Vector2{}, 48, false, Vector3{}},
			{Vector2{}, 24, false, Vector3{}}, {Vector2{}, 25, false, Vector3{}},
			{Vector2{}, 26, false, Vector3{}}, {Vector2{}, 27, false, Vector3{}},
			{Vector2{}, 55, false, Vector3{}}, {Vector2{}, 56, false, Vector3{}}
		} };

		bool has_visible = false;
		auto& camera = *Camera::GetMainCamera();

		for (auto& bone : bones) {
			auto& [screen_pos, idx, on_screen, world_pos] = bone;
			world_pos = GetBoneLocation(idx);

			if (idx == 48) world_pos.y += 0.2f;

			if (!camera.WorldToScreen(world_pos, screen_pos)) continue;

			if (screen_pos.x < 5 || screen_pos.y < 5 ||
				screen_pos.x >= Backend::width || screen_pos.y >= Backend::height)
				continue;

			bounds.left = min(bounds.left, screen_pos.x);
			bounds.right = max(bounds.right, screen_pos.x);
			bounds.top = min(bounds.top, screen_pos.y);
			bounds.bottom = max(bounds.bottom, screen_pos.y);

			on_screen = true;
			has_visible = true;
		}

		if (!has_visible)
			return { FLT_MAX, FLT_MIN, FLT_MAX, FLT_MIN };

		bounds.left -= expand;
		bounds.right += expand;
		bounds.top -= expand;
		bounds.bottom += expand;

		return bounds;
	}
	std::vector<std::pair<BoneList, Vector2>> GetValidBones() {
		static std::array<BoneList, 15> Bones = {
			l_foot, l_knee, l_hip,
			r_foot, r_knee, r_hip,
			spine1, neck, head,
			l_upperarm, l_forearm, l_hand,
			r_upperarm, r_forearm, r_hand
		};
		auto& camera = *Camera::GetMainCamera();
		std::vector<std::pair<BoneList, Vector2>> validBones;
		for (const auto& bone : Bones) {
			if (this->GetBoneLocation(bone).empty()) continue;
			if (this->GetBoneLocation(bone).Zero()) continue;

			Vector2 bonePos;

			Vector3 bone3 = this->GetBoneLocation(bone);
			if (!camera.WorldToScreen(bone3, bonePos)) continue;

			validBones.emplace_back(bone, bonePos);

		}
		return validBones;
	}
};