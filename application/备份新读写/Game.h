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
void DrawSkeleton(DWORD_PTR mesh);
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
#define Component2World  0x250
#define Id_Offset 0x24
#define RootComp 0x210
#define ActorPos 0x260
#define pUItemOffset 0x5E0
#define BoneArrayOffset 0xA00
#define Offset_Fov 0x1B64
#define Offset_Location 0x1B70
#define Offset_Rotation 0x1B50
#define name_truck 0x4180

//#define Team 0xe28
//#define ItemRelativeLocation 0x1e8

#define ItemAbsulotePos 0x260
#define ItemArrayGroup 0x260  //0x158
#define WeaponProcess 0xb40

#define WeaponBase 0x2e8
#define WeaponSpeed 0xe60
#define Anim 0xb98
#define CRCP 0x664 
#define ComponentVelocity 0x2b0

#define Health 0x870

#endif