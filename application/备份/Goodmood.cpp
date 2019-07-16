// Goodmood.cpp: 定义控制台应用程序的入口点。
//

#include <time.h>
#include <memory.h>
#include "overlay.h"
#include "Game.h"
#include <string.h>
#include <sstream>
#include <atlconv.h>
#include "Goodmood.h"
#include "Driver.h"
#include <algorithm>
#include "atlbase.h"
#include <random>



using namespace std;
CMem *mem = NULL;
//DecStruct myDecStruct;
RECT RRc;
const uint64_t Blank_Add = 0x5C05800;

namespace global
{
	DWORD_PTR pUWorld = 0;
	DWORD_PTR pGameInstance = 0;
	DWORD_PTR pLocalPlayerArray = 0;
	DWORD_PTR pLocalPlayer = 0;
	DWORD_PTR pViewportClient = 0;
	DWORD_PTR pWorld = 0;
	DWORD_PTR playercontroller = 0;
	DWORD_PTR level = 0;
	bool bPlayer = false;
	bool bVehicle = false;
	bool bLoot = false;
	bool bAimbot = false;
	bool bWeapon = false;
	bool bMenu = true;
	bool bbox = false;
	bool bViewBox = false;
	bool bBulletSpeed = false;
	bool bNames = false;
	bool bSpeed = false;
	bool bAirItem = false;
	bool magicBullt = false;
	FCameraCacheEntry cameracache = { NULL };
}


namespace Aim
{
	DWORD_PTR AimPlayer = 0; 
	bool target = false;
	vector<float> disArray;
	vector<DWORD_PTR> aimArray;
	vector<Vector3> sposArray;
	bool Lock = false;
	int locking = Bones::forehead;
	Vector3 delta;
}

namespace Actor {
	DWORD_PTR enlist = 0;
	uint32_t count = 0;
	DWORD_PTR AcknowledgedPawn = 0;
	std::map<DWORD_PTR, DWORD_PTR> rootcmp;
	DWORD_PTR camera = 0;
	std::vector<DWORD_PTR> TeamAdd;
}


WPARAM MainLoop();
void ESP();
void render();
void SetupWindow();


VOID HookJmp(ULONG64 GameAddress, ULONG64 MyAddress)
{
	unsigned char Jmp[5] = { 0xE9, 0x00, 0x00, 0x00, 0x00};
	*(uint32_t *)(Jmp + 1) = (uint32_t)(MyAddress - GameAddress - 5);
	mem->WPM(GameAddress, Jmp, sizeof(Jmp));
}

unsigned char NameSave[5] = { 0 };
unsigned char ActorSave[5] = { 0 };
unsigned char CameraSave[5] = { 0 };
unsigned char LocalSave[5] = { 0 };

//xen在这个内存页面写不进去
unsigned char NameShellcode[] = {
	0x48,0x89,0x15,0x00,0x00,0x00,0x00,  //rdx
	0x48,0x63,0xC3,
	0x48,0x63,0xCE,
	0xE9,0x00,0x00,0x00,0x00
};
unsigned char cleanName[sizeof(NameShellcode) + 8] = { 0 };


void hookGnames() {
	//unsigned char test[0x10] = { 0 };
	DWORD_PTR NameAddress = mem->GetBase()+ 0x40D465;
	mem->ReadS(NameAddress, NameSave,0x5);
	DWORD_PTR JmpAddr = mem->GetBase()+ Blank_Add;
	DWORD_PTR ReadAddr = JmpAddr+sizeof(NameShellcode);
	*(uint32_t *)(NameShellcode + 3) = (uint32_t)(ReadAddr - JmpAddr - 7);
	*(uint32_t *)(NameShellcode + 14) = (uint32_t)(NameAddress + 6 - (JmpAddr + 13) - 5);
	mem->WPM(JmpAddr, NameShellcode, sizeof(NameShellcode));
	HookJmp(NameAddress, JmpAddr);
}

void  unhookGnames() {
	DWORD_PTR NameAddress = mem->GetBase() + 0x40D465;
	mem->WPM(NameAddress, NameSave, 0x5);
	DWORD_PTR JmpAddr = mem->GetBase() + Blank_Add;
	mem->WPM(JmpAddr, cleanName, sizeof(cleanName));
}

unsigned char CameraManagerShellcode[] = {
	0x48,0x89,0x0D,0x00,0x00,0x00,0x00,
	0xE8,0x00,0x00,0x00,0x00,
	0xE9,0x00,0x00,0x00,0x00
};

unsigned char cleanCamera[sizeof(CameraManagerShellcode)+8] = { 0 };

void hookCameraManager() {
	DWORD_PTR CameraAddress = mem->GetBase() + 0x33F0464;
	mem->ReadS(CameraAddress,CameraSave, 0x5);
	DWORD_PTR JmpAddr = mem->GetBase() + Blank_Add + sizeof(NameShellcode) + 8;
	DWORD_PTR ReadAddr = JmpAddr + sizeof(CameraManagerShellcode);
	*(uint32_t *)(CameraManagerShellcode + 3) = (uint32_t)(ReadAddr - JmpAddr - 7);
	*(uint32_t *)(CameraManagerShellcode + 8) = (uint32_t)(mem->GetBase() + 0x24E07C0 - (JmpAddr + 7) - 5);
	*(uint32_t *)(CameraManagerShellcode + 13) = (uint32_t)(CameraAddress + 5 - (JmpAddr + 12) - 5);
	mem->WPM(JmpAddr, CameraManagerShellcode, sizeof(CameraManagerShellcode));
	HookJmp(CameraAddress, JmpAddr);
}

void  unhookCameraManager() {
	DWORD_PTR CameraAddress = mem->GetBase() + 0x33F0464;
	mem->WPM(CameraAddress, CameraSave, 0x5);
	DWORD_PTR JmpAddr = mem->GetBase() + Blank_Add + sizeof(NameShellcode) + 8;
	mem->WPM(JmpAddr, cleanCamera, sizeof(CameraManagerShellcode) + 8);
}



unsigned char ActorShellcode[] = {
	0x48,0x89,0x05,0x00,0x00,0x00,0x00, //rax
	0x74,0x08,
	0xB0,0x01,
	0x48,0x83,0xC4,0x20,
	0x5B,
	0xC3,
	0x32,0xC0,
	0x48,0x83,0xC4,0x20,
	0x5B,
	0xC3
};
unsigned char cleanActor[sizeof(ActorShellcode)+8] = { 0 };

void hookActor() {
	DWORD_PTR ActorAddress = mem->GetBase() + 0x268FEFE;
	mem->ReadS(ActorAddress, ActorSave, 0x5);
	DWORD_PTR JmpAddr = mem->GetBase() + Blank_Add + sizeof(NameShellcode) + 8 + sizeof(CameraManagerShellcode)+8;
	DWORD_PTR ReadAddr = JmpAddr + sizeof(ActorShellcode);
	*(uint32_t *)(ActorShellcode + 3) = (uint32_t)(ReadAddr - JmpAddr - 7);
	//*(uint32_t *)(ActorShellcode + 16) = (uint32_t)(ActorAddress + 8 - (JmpAddr + 15) - 5);
	mem->WPM(JmpAddr, ActorShellcode, sizeof(ActorShellcode));
	HookJmp(ActorAddress, JmpAddr);
}

void  unhookActor() {
	DWORD_PTR ActorAddress = mem->GetBase() + 0x268FEFE;
	mem->WPM(ActorAddress, ActorSave, 0x5);
	DWORD_PTR JmpAddr = mem->GetBase() + Blank_Add + sizeof(NameShellcode) + 8 + sizeof(CameraManagerShellcode) + 8;
	mem->WPM(JmpAddr, cleanActor, sizeof(ActorShellcode)+8);
}

//unsigned char LocalActorShellcode[] = {
//	0x48,0x89,0x05,0x00,0x00,0x00,0x00,
//	0x48,0x8B,0x4D,0xF0,
//	0x48,0x33,0xCC,
//	0xE9,0x00,0x00,0x00,0x00
//};
//
//void hookLocalPawn() {
//	DWORD_PTR LocalActor = mem->GetBase() + 0x1FE4574;
//	mem->ReadS(LocalActor, LocalSave, 0x5);
//	DWORD_PTR JmpAddr = mem->GetBase() + 0x4DE70D0 + sizeof(NameShellcode) + 8 + +sizeof(CameraManagerShellcode)+8+sizeof(ActorShellcode)+8;
//	DWORD_PTR ReadAddr = JmpAddr + sizeof(LocalActorShellcode);
//	*(uint32_t *)(LocalActorShellcode + 3) = (uint32_t)(ReadAddr - JmpAddr - 7);
//	*(uint32_t *)(LocalActorShellcode + 15) = (uint32_t)(LocalActor + 7 - (JmpAddr + 14) - 5);
//	mem->WPM(JmpAddr, LocalActorShellcode, sizeof(LocalActorShellcode));
//	HookJmp(LocalActor, JmpAddr);
//}
//
//void  unhookLocalPawn() {
//	DWORD_PTR LocalActor = mem->GetBase() + 0x1FE4574;
//	mem->WPM(LocalActor, LocalSave, 0x5);
//}

std::string Encrypt(const std::string& source) {
	int key = 0x6;
	int len = source.size();
	char* ret = new char[len + 1];
	for (int i = 0; i < len; i++) {

		ret[i] = source[i] ^ key;
		if (ret[i] == 0)
			ret[i] = source[i];
	}
	*(ret + len) = '\0';
	std::string data(ret);
	delete ret;
	return data;
}


void DrawHelp()
{
	if (GetAsyncKeyState(VK_F6) & 1) {
		//find_player_bone_offset(Actor::AcknowledgedPawn);
		global::bVehicle = global::bVehicle ? false : true;
	}
	if (GetAsyncKeyState(VK_F7) & 1)
		global::bbox = global::bbox ? false : true;
	if (GetAsyncKeyState(VK_F2) & 1) {
		//findActorSpeed();
		global::bViewBox = global::bViewBox ? false : true;
	}
	if (GetAsyncKeyState(VK_F4) & 1) {
		global::bLoot = global::bLoot ? false : true;
	}
	if (GetAsyncKeyState(VK_F1) & 1)
		global::bPlayer = global::bPlayer ? false : true;
	if (GetAsyncKeyState(VK_F3) & 1) {
		//find_offsets();
		uint8_t shell[0x57] = { 0 };
		mem->ReadS(mem->GetBase() + 0x3BF613, shell, 0x57);
		global::bAimbot = global::bAimbot ? false : true;
	}
	if (GetAsyncKeyState(VK_F8) & 1)
		global::bNames = global::bNames ? false : true;
	if (GetAsyncKeyState(VK_F9) & 1) {
		//find_Health(Actor::AcknowledgedPawn);
		global::bAirItem = global::bAirItem ? false : true;
	}
	//DrawString(RRc.left + 2, RRc.top + 20, D3DCOLOR_ARGB(200, 0x9A, 0xCD, 0x32), pFont, "骨骼透视 [F1]\n方框透视 [F2]\n超级自瞄 [F3]\n物品透视 [F4]\n完美无后 [F5]\n载具透视 [F6]\n盒子透视 [F7]\n显示名字 [F8]\n显示空投 [F9]\n瞄准部位切换 [~]\n\n       内部史迪仔Beta1.3");
}


void ESP()
{
	Aim::target = false;
	DrawHelp();
	Aim::aimArray.clear();
	Aim::sposArray.clear();
	if (GetAsyncKeyState(VK_F5) & 1) {
		cout << Actor::count << endl;
		//cout << hex << Actor::AcknowledgedPawn << endl;
		Actor::AcknowledgedPawn = 0;
		//hookLocalPawn();
		//for (int i = 0; i < 10; i++) {
		//	Actor::AcknowledgedPawn = mem->RPM<DWORD_PTR>(mem->GetBase() + 0x4DE70D0 + sizeof(NameShellcode) + 8 + sizeof(CameraManagerShellcode) + 8 + sizeof(ActorShellcode) + 8 + sizeof(LocalActorShellcode), 0x8);
		//	Sleep(50);
		//	if (Actor::AcknowledgedPawn) brweishaeak;
		//}
		//unhookLocalPawn();

		hookActor();
		for (int i = 0; i < 10; i++) {
			Actor::enlist = mem->RPM<DWORD_PTR>(mem->GetBase() + Blank_Add + sizeof(NameShellcode) + 8 + sizeof(CameraManagerShellcode) + 8 + sizeof(ActorShellcode), 0x8);
			Sleep(20);
			if (Actor::enlist) {
				break;
			}
		}
		unhookActor();

		hookCameraManager();
		for (int i = 0; i < 10; i++) {
			Actor::camera = mem->RPM<DWORD_PTR>(mem->GetBase() + Blank_Add + sizeof(NameShellcode) + 8 + sizeof(CameraManagerShellcode), 0x8);
			Sleep(20);
			if (Actor::camera) {
				break;
			}
		}
		unhookCameraManager();


		offset::Weapon_Index = 0;
		offset::Health_add.clear();
	}
	if (!Actor::enlist)
		return;
	Actor::count = mem->RPM<uint32_t>(Actor::enlist + 0x8, 0x4);
	if (Actor::count > 0 && Actor::count < 3000) {
		DWORD_PTR enlist = mem->RPM<DWORD_PTR>(Actor::enlist, 0x8);
		float health = 0.f;
		float distance = 0.f;
		global::cameracache = GetCameraCache();
		Vector3 local = global::cameracache.POV.Location;
		//if (GetAsyncKeyState(0xC0) & 1)
		//	Aim::locking = (Aim::locking == Bones::forehead) ? Bones::VB_spine_03_spine_03 : Bones::forehead;
		for (int i = 0; i < Actor::count; i++)
		{
			global::cameracache = GetCameraCache();
			auto entity = mem->RPM<DWORD_PTR>(enlist + (i * 0x8), 0x8);
			if (!entity)
				continue;
			if (entity == Actor::AcknowledgedPawn) {
				continue;
			}
			int id = tsl_decrypt_id(mem->RPM<int>(entity + Id_Offset));
			if (id == ActorIds[0] || id == ActorIds[1] || id == ActorIds[2] || id == ActorIds[3]) {
				health = mem->RPM<float>(entity + Health, 0x4);
				if (health > 0.f)
				{
					auto mesh = mem->RPM<DWORD_PTR>(entity + SkeletalMeshComponent, 0x8);
					Vector3 pos = GetBoneWithRotation(mesh, Bones::forehead);
					distance = local.Distance(pos) / 100.f;
					if (distance > 850.f)
						continue;
					if (distance <= 3 && Actor::AcknowledgedPawn == 0) {
						Actor::AcknowledgedPawn = entity;
						continue;
					}
					//if (tsl_decrypt_component(mem->RPM<uint64_t>(entity + Team)) != NULL)
					//	continue;
					Vector3 head = WorldToScreen(pos, global::cameracache);
					if (global::bPlayer) {
						FillRBG((int)head.x - 30, (int)head.y - 24, 60, 5, D3DCOLOR_ARGB(255, 0, 0, 0));
						FillRBG((int)head.x - 30, (int)head.y - 24, (int)(health * 60 / 100), 5, D3DCOLOR_ARGB(255, 0, 255, 0));
						DrawSkeleton(mesh);
						DrawString((int)head.x - 20, (int)head.y - 24, D3DCOLOR_ARGB(255, 0, 255, 0), pFont, "%d m", (int)distance);
					}
					if (global::bViewBox) {
						Vector3 Rarm = WorldToScreen(GetBoneWithRotation(mesh, 106), global::cameracache);
						Vector3 Larm = WorldToScreen(GetBoneWithRotation(mesh, 79), global::cameracache);
						Vector3 Rfoot = WorldToScreen(GetBoneWithRotation(mesh, 166), global::cameracache);
						float BoxW = Larm.x - Rarm.x;
						float BoxH = head.y - Rfoot.y;
						FillRBG((int)head.x - 30, (int)head.y - 24, 60, 5, D3DCOLOR_ARGB(255, 0, 0, 0));
						FillRBG((int)head.x - 30, (int)head.y - 24, (int)(health * 60 / 100), 5, D3DCOLOR_ARGB(255, 0, 255, 0));
						DrawBox((int)Rarm.x, (int)Rfoot.y, BoxW, BoxH + 5, D3DCOLOR_ARGB(255, 0x7A, 0x67, 0xEE));
						DrawString((int)head.x - 20, (int)head.y - 24, D3DCOLOR_ARGB(255, 0, 255, 0), pFont, "%d m", (int)distance);
					}
					if (distance < 700.f) {
						Aim::aimArray.emplace_back(entity);
						Aim::sposArray.emplace_back(pos);
						Aim::target = true;
					}
				}
				continue;
			}
			if (global::bVehicle)
			{
				if (isUaz(id)) {
					DrawVehicle(entity, local, Encrypt("含烙]#(6`[").c_str());
					continue;
				}
				else if (isDacia(id)) {
					DrawVehicle(entity, local, Encrypt("GC>0]#(6`[").c_str());
					continue;
				}
				else if (isBike(id)) {
					DrawVehicle(entity, local, Encrypt("聽酥]#(6`[").c_str());
					continue;
				}
				else if (isBuggy(id)) {
					DrawVehicle(entity, local, "山地车\n[%.0f]");
					continue;
				}
				else if (isVan(id)) {
					DrawVehicle(entity, local, Encrypt("茹卿检夸]#(6`[").c_str());
					continue;
				}
				else if (isBoat(id)) {
					DrawVehicle(entity, local, Encrypt("勃]#(6`[").c_str());
					continue;
				}
				else if (isTruck(id)) {
					DrawVehicle(entity, local, Encrypt("愧巩]#(6`[").c_str());
					continue;
				}
			}

			if (global::bAirItem && (id == KT||id==Super_KT)) {
				Vector3 pos = GetActorPos(entity);
				Vector3 spos = WorldToScreen(pos, global::cameracache);
				float dis = local.Distance(pos) / 100.f;
				DrawString((int)spos.x, (int)spos.y, D3DCOLOR_ARGB(255, 0xFF, 0x14, 0x93), pFont2, Encrypt("]#b&k[").c_str(), (int)dis);
				//DWORD_PTR AirdropItems = mem->RPM<DWORD_PTR>(entity + pUItem, 0x8);
				//int AirdropItemsCount = mem->RPM<int>(entity + pUItem+0x8, 0x4);
				//wchar_t entityname[64] = { NULL };
				//for (int i = 0; i < AirdropItemsCount; i++) {
				//	DWORD_PTR Item = mem->RPM<DWORD_PTR>(AirdropItems + i * 0x8, 0x8);
				//	DWORD_PTR ItemFString = mem->RPM<DWORD_PTR>(Item + 0x40, 0x8);  //pUItemFString 0x40
				//	DWORD_PTR ItemName = mem->RPM<DWORD_PTR>(ItemFString + 0x28, 0x8);  //pItemName 0x28
				//	ZeroMemory(entityname, sizeof(entityname));
				//	if (mem->RPMWSTR(ItemName, 0x40, entityname))
				//		DrawString(spos.x, (int)spos.y - 28 - 15 * i, D3DCOLOR_ARGB(255, 0x1F, 0x1F, 0x1F), pFont, Encrypt("]#qu[").c_str(), entityname);
				//}
				continue;
			}

			if (global::bbox && id == box) {
				int count = 0;
				Vector3 pos = GetActorPos(entity);
				Vector3 spos = WorldToScreen(pos, global::cameracache);
				float dis = local.Distance(pos) / 100.f;
				if(dis<300.f)
					DrawString((int)spos.x, (int)spos.y, D3DCOLOR_ARGB(255, 0x66, 0xCD, 0x00), pFont2, "盒子\n[%d m]", (int)dis);
				continue;
				//DWORD_PTR AirdropItems = mem->RPM<DWORD_PTR>(entity + pUItemOffset, 0x8);
				//int AirdropItemsCount = mem->RPM<int>(entity + pUItemOffset + 0x8, 0x4);
				//for (int i = 0; i < AirdropItemsCount; i++) {
				//	DWORD_PTR ItemBox = mem->RPM<DWORD_PTR>(AirdropItems + i * 0x8, 0x8);
				//	int ItemId = mem->RPM<int>(ItemBox + Id_Offset, 0x4);
				//	if ((iter = Item.find(ItemId)) != Item.end()) {
				//		DrawString(spos.x, (int)spos.y - 28 - 15 * count, iter->second.second, pFont2, "%s", Encrypt(iter->second.first.c_str()));
				//		count++;
				//	}
				//}
			}

			if (global::bLoot && (id == itemtype[0] || id == itemtype[1])) {
				//find_itm_offset(entity);
				auto DroppedItemGroupArray = mem->RPM<TArray>(entity + ItemArrayGroup); //DroppedItemGroup
				int count = DroppedItemGroupArray.count;   //DroppedItemGroup_Count
				if (!count)
					continue;
				for (int j = 0; j < count; j++)
				{
					DWORD_PTR pADroppedItemGroup = mem->RPM<DWORD_PTR>(DroppedItemGroupArray.addr + j * 0x8, 0x8);
					//find_item_loc(pADroppedItemGroup);
					DWORD_PTR pUItem = mem->RPM<DWORD_PTR>(pADroppedItemGroup + pUItemOffset, 0x8);  // DROPPEDITEMCOMPONENT_ITEM
					int32_t ItemId = tsl_decrypt_id(mem->RPM<int32_t>(pUItem + Id_Offset, 0x4));
					if ((iter = Item.find(ItemId)) != Item.end()) {
						Vector3 ItemAbPos = mem->RPM<Vector3>(pADroppedItemGroup + ItemAbsulotePos);

						//Vector3 relative = mem->RPM<Vector3>(pADroppedItemGroup + ItemRelativeLocation, 0xC); //RelativeLocation
						//Vector3 pos = relative + GetActorPos(entity);
						Vector3 screenloc = WorldToScreen(ItemAbPos, global::cameracache);
						DrawString((int)screenloc.x, (int)screenloc.y, iter->second.second, pFont2, "%s\n%d", Encrypt(iter->second.first.c_str()), (int)(local.Distance(ItemAbPos) / 100.f));
					}
				}
				continue;
			}
		}
		if (Aim::target&&global::bAimbot) {
			Vector3 spos = WorldToScreen(Aim::sposArray[0], global::cameracache);
			int len = Aim::aimArray.size();
			int minIndex = 0;
			float minx = (spos.x - s_widthb)*(spos.x - s_widthb);
			float miny = (spos.y - s_heightb)*(spos.y - s_heightb);
			float tempX = 0.f, tempY = 0.f;
			for (int i = 1; i < len; i++) {
				spos = WorldToScreen(Aim::sposArray[i], global::cameracache);
				tempX = (spos.x - s_widthb)*(spos.x - s_widthb);
				tempY = (spos.y - s_heightb)*(spos.y - s_heightb);
				if (minx + miny > tempX + tempY) {
					minIndex = i;
					minx = tempX;
					miny = tempY;;
				}
			}
			if (!Aim::Lock)
				Aim::AimPlayer = Aim::aimArray[minIndex];
			if ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) && Aim::AimPlayer != 0) {
				//if (GetAsyncKeyState(VK_F4) & 0x8000) {
				//	Actor::TeamAdd.push_back(Aim::AimPlayer);
				//}
				Aim::Lock = true;
				auto mesh = mem->RPM<DWORD_PTR>(Aim::AimPlayer + SkeletalMeshComponent, 0x8);
				Vector3 Bonebuffer = GetBoneWithRotation(mesh, Aim::locking);
				auto root = GetComponent(Aim::AimPlayer);
				Vector3 velocity = mem->RPM<Vector3>(root + ComponentVelocity);//ComponentVelocity
				cout << velocity.x << endl;
				cout << velocity.y << endl;
				cout << velocity.z << endl;
				DWORD_PTR Weapon_Processor = mem->RPM<DWORD_PTR>(Actor::AcknowledgedPawn + WeaponProcess);//WEAPONPROCESSOR
				int Weapon_Index = mem->RPM<int>(Weapon_Processor + WeaponBase + 0x20);//CurrentWeaponIndex
				if (Weapon_Index >= 0 && Weapon_Index <= 2)
				{
					DWORD_PTR Weapon_Base = mem->RPM<DWORD_PTR>(Weapon_Processor + WeaponBase);//EquippedWeapons
					DWORD_PTR CurrentWeapon = mem->RPM<DWORD_PTR>(Weapon_Base + Weapon_Index * 8);
					if (CurrentWeapon) {
						float sp = mem->RPM<float>(CurrentWeapon + WeaponSpeed, 0x4); //FTrajectoryWeaponData
						cout << sp << endl;
						float dis = local.Distance(Bonebuffer) / 100.f;
						float time = dis / sp;
						Bonebuffer.x = Bonebuffer.x + velocity.x *(time + 0.05);
						Bonebuffer.y = Bonebuffer.y + velocity.y *(time + 0.05);
						Bonebuffer.z = Bonebuffer.z + velocity.z *(time + 0.05);
						if (dis > 180.f&&dis < 260.f)
							Bonebuffer.z = Bonebuffer.z + 200.f * pow(time, 2);
						else if (dis >= 260.f&&dis < 350.f)
							Bonebuffer.z = Bonebuffer.z + 250.f * pow(time, 2);
						else if (dis >= 350.f)
							Bonebuffer.z = Bonebuffer.z + 350.f * pow(time, 2);
					}
				}
				DWORD_PTR USkeletalMeshComponent = mem->RPM<DWORD_PTR>(Actor::AcknowledgedPawn + SkeletalMeshComponent, 0x8);
				DWORD_PTR AnimScriptInstance = mem->RPM<DWORD_PTR>(USkeletalMeshComponent + Anim, 0x8);
				Vector3 ControlRotation_CP = mem->RPM<Vector3>(AnimScriptInstance + CRCP, 0xC);
				FCameraCacheEntry ce = GetCameraCache();
				ce.POV.Rotation = ControlRotation_CP;
				Vector3 BoneSpos = WorldToScreen(Bonebuffer, ce);
				if (BoneSpos.x < s_width && BoneSpos.x>0 && BoneSpos.y>0 && BoneSpos.y < s_height) {
					Aim::delta.x = BoneSpos.x - s_widthb;
					Aim::delta.y = BoneSpos.y - s_heightb;
					mouse_event(MOUSEEVENTF_MOVE, Aim::delta.x, Aim::delta.y, NULL, NULL);
					Sleep(20);
				}
			}
			else {
				Aim::Lock = false;
			}
		}
	}
	
}


//render function
void render()
{

	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	d3ddev->BeginScene();    
	ESP();
	d3ddev->EndScene();  
	d3ddev->Present(NULL, NULL, NULL, NULL);  
}

//set up overlay window
void SetupWindow()
{
	RECT rc;
	twnd = FindWindowA(Encrypt("ShtcgjQohbiq").c_str(),0);
	if (twnd != NULL)
	{
		GetWindowRect(twnd, &rc);
		s_width = rc.right - rc.left;
		s_height = rc.bottom - rc.top;
	}
	else
	{
		Sleep(3000);
		ExitProcess(0);
	}
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(0);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(0x1000, 0x4000);
	int randNumber = dis(gen);
	wc.hbrBackground = (HBRUSH)RGB(0, 0, 0);
	char szbuff[200] = { 0 };
	_itoa_s(randNumber, szbuff, 10);
	wc.lpszClassName = CA2W(szbuff);
	RegisterClassEx(&wc);

	hWnd = CreateWindowExW(NULL, wc.lpszClassName, wc.lpszClassName, WS_POPUP | WS_VISIBLE, rc.left, rc.top, s_width, s_height, NULL, NULL, wc.hInstance, NULL);
	SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST | WS_POPUP | WS_EX_LAYERED | WS_EX_TRANSPARENT| WS_EX_TOOLWINDOW);

	SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 0, ULW_COLORKEY);
	SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);

	ShowWindow(hWnd, SW_SHOW);
	initD3D(hWnd);
}

WPARAM MainLoop()
{
	MSG msg;
	RECT rc;
	while (TRUE)
	{
		ZeroMemory(&msg, sizeof(MSG));
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			exit(0);
		if (!twnd)
		{
			ExitProcess(0);
		}
		ZeroMemory(&rc, sizeof(RECT));
		GetWindowRect(twnd, &RRc);
		s_width = RRc.right - RRc.left;
		s_height = RRc.bottom - RRc.top;
		MoveWindow(hWnd, RRc.left, RRc.top, s_width, s_height, true);
		SetWindowPos(hWnd, GetWindow(twnd, GW_HWNDPREV), 0, 0, 0, 0, SWP_NOMOVE || SWP_NOSIZE);
		UpdateWindow(hWnd);
		Sleep(2);
		render();
	}
	return msg.wParam;
}




DWORD GetProcessIDFromName()
{
	HANDLE snapshot;
	PROCESSENTRY32 processinfo;
	processinfo.dwSize = sizeof(processinfo);
	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == NULL)
		return FALSE;
	BOOL status = Process32First(snapshot, &processinfo);
	while (status)
	{
		if (12725395128944665054 == WHashfun(processinfo.szExeFile) && processinfo.cntThreads>20)
			return processinfo.th32ProcessID;
		status = Process32Next(snapshot, &processinfo);
	}
	return -1;
}
void GetAppPathA(char *szCurFile) //最后带斜杠
{
	GetModuleFileNameA(0, szCurFile, MAX_PATH);
	for (SIZE_T i = strlen(szCurFile) - 1; i >= 0; i--)
	{
		if (szCurFile[i] == '\\')
		{
			szCurFile[i + 1] = '\0';
			break;
		}
	}
}
bool DriverInstall() {
	char szSysFile[MAX_PATH] = { 0 };
	char szDisplay[] = "PdX03gA3p2";
	GetAppPathA(szSysFile);
	strcat(szSysFile, "PdX03gA3p2.sys");
	cDrvCtrl* u_dc = new cDrvCtrl();
	if (!u_dc->Install(szSysFile, szDisplay, szDisplay))
		return false;
	if (!u_dc->Start())
		return false;
	delete u_dc;
	return true;
}

ofstream logfile;
std::map<int, uint64_t> m_map;
std::map<uint64_t, uint64_t> v_map;
int main()
{
	DWORD pid = NULL;
	if (!DriverInstall()) {
		cout << "启动失败" << endl;
		system("pause");
		return 0;
	}
	while (((pid = GetProcessIDFromName()) == -1)) {
		Sleep(2000);
	}
	//std::cout << pid << std::endl;
	Sleep(21000);
	mem = new CMem(pid);
	//std::cout <<std::hex<< mem->GetBase() << std::endl;
	//logfile.open("mylog.txt");
	//hookActor();
	//unhookActor();
	//hookCameraManager();
	hookGnames();
	CacheNames();
	SetupWindow();
	MainLoop();
	system("pause");
    return 0;
}