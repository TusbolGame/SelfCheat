#pragma once
#ifndef HEAD_GOODMOOD_H
#define HEAD_GOODMOOD_H
#include <Windows.h>
#include <Psapi.h>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <functional>
#include <vector>
#include <map>
#include <list>
#include <mutex>
#include <queue>
#include <sstream>
#include <atomic>
#include "overlay.h"
#include "KReader.hpp"



class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f)
	{
	}

	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
	{
	}
	~Vector3()
	{
	}

	float x;
	float y;
	float z;

	inline float Dot(Vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline float Distance(Vector3 v)
	{
		return float(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}

	Vector3 operator+(Vector3 v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator-(Vector3 v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	inline bool empty() {
		return (abs(x) < 1e-6 && abs(y) < 1e-6 && abs(z) < 1e-6);
	}
};

struct FQuat
{
	float x;
	float y;
	float z;
	float w;
};

struct FTransform
{
	FQuat rot;
	Vector3 translation;
	char pad[4];
	Vector3 scale;
	char pad1[4];
	D3DMATRIX ToMatrixWithScale()
	{
		D3DMATRIX m;
		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;

		float x2 = rot.x + rot.x;
		float y2 = rot.y + rot.y;
		float z2 = rot.z + rot.z;

		float xx2 = rot.x * x2;
		float yy2 = rot.y * y2;
		float zz2 = rot.z * z2;
		m._11 = (1.0f - (yy2 + zz2)) * scale.x;
		m._22 = (1.0f - (xx2 + zz2)) * scale.y;
		m._33 = (1.0f - (xx2 + yy2)) * scale.z;

		float yz2 = rot.y * z2;
		float wx2 = rot.w * x2;
		m._32 = (yz2 - wx2) * scale.z;
		m._23 = (yz2 + wx2) * scale.y;

		float xy2 = rot.x * y2;
		float wz2 = rot.w * z2;
		m._21 = (xy2 - wz2) * scale.y;
		m._12 = (xy2 + wz2) * scale.x;

		float xz2 = rot.x * z2;
		float wy2 = rot.w * y2;
		m._31 = (xz2 + wy2) * scale.z;
		m._13 = (xz2 - wy2) * scale.x;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};


typedef struct FTransform2
{
	D3DXVECTOR4 Rotation;
	D3DXVECTOR3 Translation;
	D3DXVECTOR3 Scale3D;
};

//Required PUBG classes


struct FMinimalViewInfo
{
	class Vector3                                      Location;                                                 // 0x0000(0x000C) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	class Vector3                                      Rotation;                                                 // 0x000C(0x000C) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              FOV;                                                      // 0x0018(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
};



struct FCameraCacheEntry
{
	float                                              TimeStamp;                                                // 0x0000(0x0004) (CPF_ZeroConstructor, CPF_IsPlainOldData)
	unsigned char                                      UnknownData00[0xC];                                       // 0x0004(0x000C) MISSED OFFSET
	struct FMinimalViewInfo                            POV;                                                      // 0x0010(0x0530)
};

enum Bones : int
{
	Root = 0,
	pelvis = 1,
	spine_01 = 2,
	spine_02 = 3,
	spine_03 = 4,
	neck_01 = 5,
	Head = 6,
	face_root = 7,
	eyebrows_pos_root = 8,
	eyebrows_root = 9,
	eyebrows_r = 10,
	eyebrows_l = 11,
	eyebrow_l = 12,
	eyebrow_r = 13,
	forehead_root = 14,
	forehead = 15,
	jaw_pos_root = 16,
	jaw_root = 17,
	jaw = 18,
	mouth_down_pos_root = 19,
	mouth_down_root = 20,
	lip_bm_01 = 21,
	lip_bm_02 = 22,
	lip_br = 23,
	lip_bl = 24,
	jaw_01 = 25,
	jaw_02 = 26,
	foretou=18,
	cheek_pos_root = 27,
	cheek_root = 28,
	cheek_r = 29,
	cheek_l = 30,
	nose_side_root = 31,
	nose_side_r_01 = 32,
	nose_side_r_02 = 33,
	nose_side_l_01 = 34,
	nose_side_l_02 = 35,
	eye_pos_r_root = 36,
	eye_r_root = 37,
	eye_rot_r_root = 38,
	eye_lid_u_r = 39,
	eye_r = 40,
	eye_lid_b_r = 41,
	eye_pos_l_root = 42,
	eye_l_root = 43,
	eye_rot_l_root = 44,
	eye_lid_u_l = 45,
	eye_l = 46,
	eye_lid_b_l = 47,
	nose_pos_root = 48,
	nose = 49,
	mouth_up_pos_root = 50,
	mouth_up_root = 51,
	lip_ul = 52,
	lip_um_01 = 53,
	lip_um_02 = 54,
	lip_ur = 55,
	lip_l = 56,
	lip_r = 57,
	hair_root = 58,
	hair_b_01 = 59,
	hair_b_02 = 60,
	hair_l_01 = 61,
	hair_l_02 = 62,
	hair_r_01 = 63,
	hair_r_02 = 64,
	hair_f_02 = 65,
	hair_f_01 = 66,
	hair_b_pt_01 = 67,
	hair_b_pt_02 = 68,
	hair_b_pt_03 = 69,
	hair_b_pt_04 = 70,
	hair_b_pt_05 = 71,
	camera_fpp = 72,
	GunReferencePoint = 73,
	GunRef = 74,
	breast_l = 85,
	breast_r = 76,
	clavicle_l = 77,
	upperarm_l = 78,
	lowerarm_l = 79,
	hand_l = 80,
	thumb_01_l = 81,
	thumb_02_l = 82,
	thumb_03_l = 83,
	thumb_04_l_MBONLY = 84,
	index_01_l = 85,
	index_02_l = 86,
	index_03_l = 87,
	index_04_l_MBONLY = 88,
	middle_01_l = 89,
	middle_02_l = 90,
	middle_03_l = 91,
	middle_04_l_MBONLY = 92,
	ring_01_l = 93,
	ring_02_l = 94,
	ring_03_l = 95,
	ring_04_l_MBONLY = 96,
	pinky_01_l = 97,
	pinky_02_l = 98,
	pinky_03_l = 99,
	pinky_04_l_MBONLY = 100,
	item_l = 101,
	lowerarm_twist_01_l = 102,
	upperarm_twist_01_l = 103,
	clavicle_r = 104,
	upperarm_r = 105,
	lowerarm_r = 106,
	hand_r = 107,
	thumb_01_r = 108,
	thumb_02_r = 109,
	thumb_03_r = 110,
	thumb_04_r_MBONLY = 111,
	index_01_r = 112,
	index_02_r = 113,
	index_03_r = 114,
	index_04_r_MBONLY = 115,
	middle_01_r = 116,
	middle_02_r = 117,
	middle_03_r = 118,
	middle_04_r_MBONLY = 119,
	ring_01_r = 120,
	ring_02_r = 121,
	ring_03_r = 122,
	ring_04_r_MBONLY = 123,
	pinky_01_r = 124,
	pinky_02_r = 125,
	pinky_03_r = 126,
	pinky_04_r_MBONLY = 127,
	item_r = 128,
	lowerarm_twist_01_r = 129,
	upperarm_twist_01_r = 130,
	BackPack = 131,
	backpack_01 = 132,
	backpack_02 = 133,
	Slot_Primary = 134,
	Slot_Secondary = 135,
	Slot_Melee = 136,
	slot_throwable = 137,
	coat_l_01 = 138,
	coat_l_02 = 139,
	coat_l_03 = 140,
	coat_l_04 = 141,
	coat_fl_01 = 142,
	coat_fl_02 = 143,
	coat_fl_03 = 144,
	coat_fl_04 = 145,
	coat_b_01 = 146,
	coat_b_02 = 147,
	coat_b_03 = 148,
	coat_b_04 = 149,
	coat_r_01 = 150,
	coat_r_02 = 151,
	coat_r_03 = 152,
	coat_r_04 = 153,
	coat_fr_01 = 154,
	coat_fr_02 = 155,
	coat_fr_03 = 156,
	coat_fr_04 = 157,
	thigh_l = 158,
	calf_l = 159,
	foot_l = 160,
	ball_l = 161,
	calf_twist_01_l = 162,
	thigh_twist_01_l = 163,
	thigh_r = 164,
	calf_r = 165,
	foot_r = 166,
	ball_r = 167,
	calf_twist_01_r = 168,
	thigh_twist_01_r = 169,
	Slot_SideArm = 170,
	skirt_l_01 = 171,
	skirt_l_02 = 172,
	skirt_l_03 = 173,
	skirt_f_01 = 174,
	skirt_f_02 = 175,
	skirt_f_03 = 176,
	skirt_b_01 = 177,
	skirt_b_02 = 178,
	skirt_b_03 = 179,
	skirt_r_01 = 180,
	skirt_r_02 = 181,
	skirt_r_03 = 182,
	ik_hand_root = 183,
	ik_hand_gun = 184,
	ik_hand_r = 185,
	ik_hand_l = 186,
	ik_aim_root = 187,
	ik_aim_l = 188,
	ik_aim_r = 189,
	ik_foot_root = 190,
	ik_foot_l = 191,
	ik_foot_r = 192,
	camera_tpp = 193,
	ik_target_root = 194,
	ik_target_l = 195,
	ik_target_r = 196,
	VB_spine_03_spine_03 = 197,
	VB_upperarm_r_lowerarm_r = 198
};

//declarations
namespace global
{
	extern DWORD_PTR pUWorld;
	extern DWORD_PTR pGameInstance;
	extern DWORD_PTR pLocalPlayerArray;
	extern DWORD_PTR pLocalPlayer;
	extern DWORD_PTR pViewportClient;
	extern DWORD_PTR pWorld;
	extern DWORD_PTR playercontroller;
	extern DWORD_PTR level;
	extern bool bPlayer;
	extern bool bVehicle;
	extern bool bLoot;
	extern bool bAimbot;
	extern bool bWeapon;
	extern bool bMenu;
	extern bool bbox;
	extern bool bViewBox;
	extern bool bBulletSpeed;
	extern bool bNames;
	extern bool bSpeed;
	extern bool bAirItem;
	extern bool magicBullt;
	extern bool bUpdate;
	extern FCameraCacheEntry cameracache;
}


namespace Aim
{
	extern DWORD_PTR AimPlayer;
	extern bool target;
	extern vector<float> disArray;
	extern vector<DWORD_PTR> aimArray;
	extern vector<Vector3> sposArray;
	extern bool Lock;
	extern int32_t click;    //平滑比例系数
	extern DWORD_PTR BeforeAim;
	extern bool is_no_rcl; //压枪设置
	extern SIZE_T recoil_total;//后座总数
	extern queue<uint64_t> recoil_quene;//后座系数队列
	extern Vector3 delta;
}


namespace Actor {
	extern DWORD_PTR enlist;
	extern uint32_t count;
	extern DWORD_PTR AcknowledgedPawn;
	extern std::map<DWORD_PTR, DWORD_PTR> rootcmp;
	extern DWORD_PTR camera;
	extern std::vector<DWORD_PTR> TeamAdd;
}



namespace offset {
	extern uint32_t Bone_Array;
	extern uint32_t Weapon_Processor;
	extern uint32_t Weapon_Base;
	extern uint32_t Weapon_Speed;
	extern uint32_t Weapon_Index;
	extern uint32_t AnimScriptInstance;
	extern uint32_t Rotation_Crcp;
	extern std::vector<uint64_t> Health_add;
}

extern SIZE_T ActorIds[4];
extern SIZE_T uaz[3];
extern SIZE_T  dacia[4];
extern SIZE_T  motorbike[7];
extern SIZE_T buggy[7];
extern SIZE_T  boat;
extern SIZE_T itemtype[2];
extern SIZE_T  KT;
extern SIZE_T Super_KT;
extern SIZE_T box;
extern SIZE_T Van[3];
extern SIZE_T  pK[10];
extern SIZE_T IteamWant[23];
extern hash<string> Hashfunc;
extern hash<wstring> WHashfun;
extern map<int32_t, pair<string, DWORD>> Item; 
extern map<int32_t, pair<string, DWORD>>::iterator iter;
extern map<int, float> m_98K;
extern CMem* mem;
extern std::map<int, uint64_t> m_map;
extern std::map<uint64_t, uint64_t> v_map;


struct TArray {
	uint64_t addr;
	uint32_t count;
};
#endif