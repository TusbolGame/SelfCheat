#pragma once
#ifndef HEAD_Game_H
#define HEAD_Game_H
#include <d3d9.h>
#include <d3dx9.h>
#include <Dwmapi.h> 
#include <TlHelp32.h>
#include "Goodmood.h"
void UpdateAddresses();
bool CacheNames();
void InitGNAMES();
Vector3 GetActorPos(const DWORD_PTR& entity);
float GetActorHealth(const DWORD_PTR& pAActor);
int isUaz(int id);
int isDacia(int id);
int isBuggy(int id);
int isBike(int id);
int isBoat(int id);
int isVan(int id);
int isTruck(int id);
void DrawSkeleton(const DWORD_PTR& mesh, Sex sex);
void DrawVehicle(const DWORD_PTR& entity, Vector3& local, const char* txt);
Vector3 WorldToScreen(Vector3 WorldLocation, FCameraCacheEntry CameraCacheL);
FCameraCacheEntry GetCameraCache();
Vector3 GetBoneWithRotation(DWORD_PTR mesh, int id);
std::string GetNameFromId(const int& ID);
Vector3 Clamp(Vector3 Ver);
//Vector3 ToFRotator(Vector3 Ver);
DWORD_PTR Get_World();
DWORD_PTR Get_AActor(DWORD_PTR leve);
DWORD_PTR Get_Encry(DWORD_PTR point);
DWORD_PTR GetComponent(const DWORD_PTR& entity);
void  unhookGnames();


typedef void(__fastcall *decryptPTR)();

typedef struct DecryptSTR {
	decryptPTR decActor;
	decryptPTR decProp;
	decryptPTR decGnames;
}DecStruct,*PDecStruct;

//extern DecStruct myDecStruct;

struct uint128_t {
	uint64_t low;
	uint64_t high;
};
uint64_t tsl_decrypt_prop(struct tsl *tsl,uint64_t prop);
uint64_t tsl_decrypt_actor(struct tsl *tsl,uint64_t actor);
uint32_t tsl_decrypt_id(const int& id);
uint64_t tsl_decrypt_prop(uint64_t prop);
uint64_t tsl_decrypt_actor(uint64_t prop);
uint64_t tsl_decrypt_component(const uint64_t& root);
int tsl_init(struct tsl *tsl);
void tsl_finit(struct tsl *tsl);
typedef uint64_t(*decrypt_func)(uint64_t);
struct tsl {
	decrypt_func func;
};


//offset
extern ofstream logfile;
extern DWORD_PTR GNames;
void find_Health(const DWORD_PTR& myplayer);
void findActorSpeed();
void find_player_bone_offset(const uint64_t& _cur_mesh_address);
void find_offsets();
void find_itm_offset(const uint64_t& _address);
void find_item_loc(const uint64_t& _address);

bool tsl_init(PDecStruct decPtrst);

extern  const uint64_t Blank_Add;
#define SkeletalMeshComponent 0x508
#define Component2World  0x320
#define Id_Offset 0x20
#define RootComp 0x350
#define ActorPos 0x330
#define pUItemOffset 0x610
#define BoneArrayOffset 0xA20
#define Offset_Fov 0x9A0
#define Offset_Location 0x9B4
#define Offset_Rotation 0x454

#define name_truck 0x41BC

//#define Team 0xe28
#define ItemRelativeLocation 0x1F0

#define ItemArrayGroup 0x2E8
//#define ItemAbsulotePos 0x354



#define WeaponProcess 0x15F0
#define WeaponBase 0x2A0
#define WeaponSpeed 0xCA8
#define Anim 0xBB8
#define CRCP 0x680
#define ComponentVelocity 0x20C

#define Health 0xC08

#define offset_weapon_index 0x21

extern "C" uint64_t __fastcall asm_decrypt_component(uint64_t root);


#endif