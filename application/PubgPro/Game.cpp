#include "Game.h"




#define Pi 3.1415926535

using namespace std;
int s_width = 1920;
int s_height = 1080;
int s_widthb = 960;
int s_heightb = 540;
LPDIRECT3D9 d3d;
LPDIRECT3DDEVICE9 d3ddev;
HWND hWnd;
HWND twnd;
MARGINS  margin = { 0,0,s_width,s_height };
LPD3DXFONT pFont;
LPD3DXFONT pFont2;
ID3DXLine* d3dLine;
hash<string> Hashfunc;
hash<wstring> WHashfun;
map<int32_t, pair<string, DWORD>> Item;
map<int32_t, pair<string, DWORD>>::iterator iter;
map<int, float> m_98K;
SIZE_T ActorIds[4];
SIZE_T uaz[3];
SIZE_T dacia[4];
SIZE_T motorbike[7];
SIZE_T buggy[7];
SIZE_T boat = 0;
SIZE_T itemtype[2];
SIZE_T KT = 0;
SIZE_T box = 0;
SIZE_T Van[3];
SIZE_T pK[10];
SIZE_T Super_KT = 0;
DWORD_PTR GNames = 0;
DWORD_PTR Camera = 0;

list<int> upper_part = { Bones::neck_01, Bones::Head, Bones::forehead };
list<int> right_arm = { Bones::neck_01, Bones::upperarm_r, Bones::lowerarm_r, Bones::hand_r };
list<int> left_arm = { Bones::neck_01, Bones::upperarm_l, Bones::lowerarm_l, Bones::hand_l };
list<int> spine = { Bones::neck_01, Bones::spine_02, Bones::spine_01, Bones::pelvis };

list<int> lower_right = { Bones::pelvis, Bones::thigh_r, Bones::calf_r, Bones::foot_r };
list<int> lower_left = { Bones::pelvis, Bones::thigh_l, Bones::calf_l, Bones::foot_l };

list<list<int>> skeleton = { upper_part, right_arm, left_arm, spine, lower_right, lower_left };



list<int> famale_upper_part = { Bones::neck_01, Bones::Head, Bones::forehead };
list<int> famale_right_arm = { Bones::neck_01, Bones::index_04_r_MBONLY, Bones::middle_01_r, Bones::middle_02_r };
list<int> famale_left_arm = { Bones::neck_01, Bones::index_04_l_MBONLY, Bones::middle_01_l, Bones::middle_02_l };
list<int> famale_spine = { Bones::neck_01, Bones::spine_02, Bones::spine_01, Bones::pelvis };
list<int> famale_lower_right = { Bones::pelvis, Bones::skirt_f_01, Bones::skirt_f_02, Bones::skirt_f_03 };
list<int> famale_lower_left = { Bones::pelvis, Bones::calf_twist_01_r, Bones::thigh_twist_01_r, Bones::Slot_SideArm };

list<list<int>> famale_skeleton = { famale_upper_part, famale_right_arm, famale_left_arm, famale_spine, famale_lower_right, famale_lower_left };




BYTE shellcode1[40] = {
	0x48,0x83,0xec,0x28,
	0x48,0x8D,0x0d,0x00,0x00,0x00,0x00,
	0xe8,0x00,0x00,0x00,0x00,
	0x48,0x89,0x05,0x05,0x00,0x00,0x00,
	0x48,0x83,0xc4,0x28,
	0xc3,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};


BYTE clean[40] = { NULL };
void Set_wHook()
{
	DWORD_PTR world_Addr = mem->GetBase() + 0x38D2580;
	*(DWORD*)(shellcode1 + 7) = (mem->GetBase() + 0x41CDB30) - (world_Addr + 4) - 7;
	*(DWORD*)(shellcode1 + 12) = (mem->GetBase() + 0x26DE60) - (world_Addr + 11) - 5; 
	mem->WPM(world_Addr, shellcode1, sizeof(shellcode1));
}

DWORD_PTR Get_World()
{
	Set_wHook();
	DWORD_PTR world_Addr = mem->GetBase() + 0x38D2580;
	mem->MCreateThread(world_Addr);
	DWORD_PTR world = mem->RPM<DWORD_PTR>(world_Addr + 28, sizeof(DWORD_PTR));
	mem->WPM(world_Addr,clean, sizeof(shellcode1));
	if (world)
	{
		return world;
	}
	return 0;
}



BYTE shellcode2[40] = {
	0x48,0x83,0xec,0x28,
	0x48,0xb9,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xe8,0x00,0x00,0x00,0x00,
	0x48,0x89,0x05,0x05,0x00,0x00,0x00,
	0x48,0x83,0xc4,0x28,
	0xc3,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
void Set_AHook()
{
	DWORD_PTR AActor_Addr = mem->GetBase() + 0x38D2580;
	*(DWORD*)(shellcode2 + 15) = (mem->GetBase() + 0x1270F30) - (AActor_Addr + 14) - 5; 
	mem->WPM(AActor_Addr, shellcode2, sizeof(shellcode2));
}


DWORD_PTR Get_AActor(DWORD_PTR leve)
{
	Set_AHook();
	DWORD_PTR AActor_Addr = mem->GetBase() + 0x38D2580;
	mem->WPM(AActor_Addr + 6, &leve, sizeof(DWORD_PTR));
	mem->MCreateThread(AActor_Addr);
	DWORD_PTR AActor = mem->RPM<DWORD_PTR>(AActor_Addr + 31, sizeof(DWORD_PTR));
	mem->WPM(AActor_Addr, clean, sizeof(shellcode2));
	if (AActor)
	{
		return AActor;
	}
	return 0;
}


byte shellcode3[40] = {
	0x48,0x83,0xec,0x28,
	0xe8,0x00,0x00,0x00,0x00,
	0x48,0x8b,0xc8,  //mov rcx,rax
	0xe8,0x00,0x00,0x00,0x00,
	0x48,0x89,0x05,0x08,0x00,0x00,0x00,
	0x48,0x83,0xc4,0x28, //add rsp
	0xc3,
	0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };

void Set_NHook()
{
	DWORD_PTR Name_Addr = mem->GetBase() + 0x38D2580;
	*(DWORD*)(shellcode3 + 5) = (mem->GetBase() + 0xAE0400) - (Name_Addr + 4) - 5; 
	*(DWORD*)(shellcode3 + 13) = (mem->GetBase() + 0x2CFB50) - (Name_Addr + 12) - 5;
	mem->WPM(Name_Addr, shellcode3, sizeof(shellcode3));
}

DWORD_PTR getname()
{
	Set_NHook();
	DWORD_PTR Name_Addr = mem->GetBase() + 0x38D2580;
	mem->MCreateThread(Name_Addr);
	DWORD_PTR Name = mem->RPM<DWORD_PTR>(Name_Addr + 32, sizeof(DWORD_PTR));
	mem->WPM(Name_Addr, clean, sizeof(shellcode3));
	if (Name)
	{
		return Name;
	}
	return 0;
}



BYTE shellcode4[40] = {
	0x48,0x83,0xec,0x28,
	0x48,0xb9,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xe8,0x00,0x00,0x00,0x00,
	0x48,0x89,0x05,0x05,0x00,0x00,0x00,
	0x48,0x83,0xc4,0x28,
	0xc3,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
void Set_OHook()
{
	DWORD_PTR Encry_Addr = mem->GetBase() + 0x38D2580;
	*(DWORD*)(shellcode4 + 15) = (mem->GetBase() + 0x27AD20) - (Encry_Addr + 14) - 5;
	mem->WPM(Encry_Addr, shellcode4, sizeof(shellcode4));

}


DWORD_PTR Get_Encry(DWORD_PTR point)
{
	Set_OHook();
	DWORD_PTR Encry_Addr = mem->GetBase() + 0x38D2580;
	mem->WPM(Encry_Addr + 6, &point, sizeof(DWORD_PTR));
	mem->MCreateThread(Encry_Addr);
	DWORD_PTR Encry = mem->RPM<DWORD_PTR>(Encry_Addr + 31, sizeof(DWORD_PTR));
	mem->WPM(Encry_Addr, clean, sizeof(shellcode4));
	if (Encry)
	{
		return Encry;
	}
	return 0;
}


void InitGNAMES() {
	//DWORD_PTR tmp = mem->RPM<DWORD_PTR>(mem->GetBase() + 0x4353400);
	//tmp = (tmp >> 6 * 4) << 6 * 4;
	//for (int i = 0; i < 0xFFFFF; i++)
	//{
	//	GNames = tmp + (i * 0x80);
	//	if (
	//		GetNameFromId(1) == string("ByteProperty")
	//		&& GetNameFromId(2) == string("IntProperty")
	//		&& GetNameFromId(3) == string("BoolProperty")
	//		)
	//	{
	//		cout <<hex<< GNames << endl;
	//		break;
	//	}
	//}
	GNames = 0x0000000002610080;
}




bool CacheNames()
{
	string name = "";
	SIZE_T temp = 0;
	do {
		GNames = mem->RPM<DWORD_PTR>(mem->GetBase() + Blank_Add + 18, 0x8);
		Sleep(128);
	} while (!GNames);
	unhookGnames();
	//InitGNAMES();
	for (int i = 0, c1 = 0, c2 = 0, c3 = 0, c4 = 0, c5 = 0, c6 = 0, c7 = 0, c8 = 0; i < 200000; i++)
	{
		if (c1 == 4 && c2 == 3 && c3 == 4 && c4 == 7 && c5 == 7 && c6 == 2 && c7 == 3 && c8 == 10 && KT != 0 && boat != 0 && box != 0)//&& Item.size()==22
		{
			return true;
		}
		name = GetNameFromId(i);
		//logfile << name << "                  " << i << endl;
		//cout << name << endl;
		temp = Hashfunc(name);
		if (temp == 8049205518224136996) {
			continue;
		}
		if (temp == 8813260772116751492 || temp == 18343336339496486534 || temp == 7933270624895117445 || temp == 9875268107888323191)
		{
			ActorIds[c1++] = i;
			cout << name << endl;
			continue;
		}
		if (temp == 3740972445146528971 || temp == 6913485731149500180 || temp == 13230958519645436073)
		{
			uaz[c2++] = i;
			cout << name << endl;
			continue;
		}
		if (temp == 11352407076170963838 || temp == 4813543147473565341 || temp == 8077779892506988992 || temp== 7660433767852026703)
		{
			dacia[c3++] = i;
			continue;
		}
		if (temp == 185767084334536707 || temp == 185772581892677762 || temp == 12725995461060245232 || temp == 12726003157641642709 || temp == 6321964952282369881 || temp == 7531324119730992447 || temp == 12644824015350519923)

		{
			motorbike[c4++] = i;
			cout << name << endl;
			continue;
		}
		if (temp == 9628432884676153481 || temp == 9628429586141268848 || temp == 9628430685652897059) {
			Van[c7++] = i;
			continue;
		}
		if (temp == 4040486523027163935 || temp == 4040487622538792146 || temp == 4040488722050420357 || temp == 4040481025469022880 || temp == 4040482124980651091 || temp == 7304760651455946760 || temp == 7304763949990831393 || temp == 7304762850479203182 || temp == 7304766149014087815 || temp == 7304765049502459604) {
			pK[c8++] = i;
			continue;
		}
		if (temp == 1204892574844850097 || temp == 1204889276309965464 || temp == 1204890375821593675 || temp == 1204895873379734730 || temp == 13717046390059868982 || temp == 1204896972891362941 || temp == 1204893674356478308)
		{
			buggy[c5++] = i;
			continue;
		}
		if (temp == 15970073267704214308)
		{
			boat = i;
			continue;
		}
		if (temp == 6738219194122078909 || temp == 561885865602212793)
		{
			//cout << name << endl;
			itemtype[c6++] = i;
			continue;
		}
		if (temp == 3854434236331542061) {
			KT = i;
			continue;
		}
		if (temp == 17513829798132595491) {
			box = i;
			continue;
		}
		if (temp == 8049205518224136996) {
			Super_KT = i;
			continue;
		}
		if (temp == 12299086612447800398) {
			Item.insert(make_pair(i, make_pair("Îûº°±Æ", D3DCOLOR_ARGB(255, 0xFF, 0x14, 0x93))));
			continue;
		}
		if (temp == 2045057708264032334) {
			Item.insert(make_pair(i, make_pair("°øº°±Æ", D3DCOLOR_ARGB(255, 0xEE, 0xAD, 0x0E))));
			continue;
		}
		if (temp == 4629559976768243881) {
			Item.insert(make_pair(i, make_pair("Îûº°±Æ", D3DCOLOR_ARGB(255, 0xFF, 0x14, 0x93))));
			continue;
		}
		if (temp == 4629558877256615670) {
			Item.insert(make_pair(i, make_pair("°øº°±Æ", D3DCOLOR_ARGB(255, 0xEE, 0xAD, 0x0E))));
			continue;
		}
		if (temp == 1212661687138194801) {
			Item.insert(make_pair(i, make_pair("°øº°Ë±", D3DCOLOR_ARGB(255, 0xEE, 0xAD, 0x0E))));
			continue;
		}
		if (temp == 13382025388113246472) {
			Item.insert(make_pair(i, make_pair("°øº°Ë±", D3DCOLOR_ARGB(255, 0xEE, 0xAD, 0x0E))));
			continue;
		}
		if (temp == 2240124832908017059) {
			Item.insert(make_pair(i, make_pair("Îûº°Ë±", D3DCOLOR_ARGB(255, 0xFF, 0x14, 0x93))));
			continue;
		}
		if (temp == 2672090015279184355) {
			Item.insert(make_pair(i, make_pair("ÔýÇÉ", D3DCOLOR_ARGB(255, 0xEE, 0xAD, 0x0E))));
			continue;
		}
		if (temp == 733064755015327580) {
			Item.insert(make_pair(i, make_pair("Ð¿Ë²Ô¯", D3DCOLOR_ARGB(255, 0xEE, 0xAD, 0x0E))));
			continue;
		}
		if (temp == 1610939504786002950) {
			Item.insert(make_pair(i, make_pair("º·¸Î¶ú", D3DCOLOR_ARGB(255, 0xEE, 0xAD, 0x0E))));
			continue;
		}
		if (temp == 12117696530092652984) {
			Item.insert(make_pair(i, make_pair("Ô»ÇÀÉâ", D3DCOLOR_ARGB(255, 0xEE, 0xAD, 0x0E))));
			continue;
		}
		if (temp == 14862897011202308367) {
			Item.insert(make_pair(i, make_pair("¹êÍßÆ¯ÎÛ", D3DCOLOR_ARGB(255, 0xEE, 0xAD, 0x0E))));
			continue;
		}
		if (temp == 14565114510902503375) {
			Item.insert(make_pair(i, make_pair("ÍÂ·°¸³", D3DCOLOR_ARGB(255, 0xEE, 0xAD, 0x0E))));
			continue;
		}
		if (temp == 8898064414976657127) {
			Item.insert(make_pair(i, make_pair("¶Í·°¸³", D3DCOLOR_ARGB(255, 0xEE, 0xAD, 0x0E))));
			continue;
		}
		if (temp == 16528107428588640296) {
			Item.insert(make_pair(i, make_pair("´»Á¿ÉýÔò", D3DCOLOR_ARGB(255, 0xEE, 0xAD, 0x0E))));
			continue;
		}
		if (temp == 18385222816841869748) {
			Item.insert(make_pair(i, make_pair("¸×½ñÉýÔò", D3DCOLOR_ARGB(255, 0xEE, 0xAD, 0x0E))));
			continue;
		}
		if (temp == 18328323694289543845) {
			Item.insert(make_pair(i, make_pair("GMK", D3DCOLOR_ARGB(255, 0xEE, 0xAD, 0x0E))));
			continue;
		}
		if (temp == 1400576478221848568) {
			Item.insert(make_pair(i, make_pair("Mgt?>m", D3DCOLOR_ARGB(255, 0xFF, 0x14, 0x93))));
			continue;
		}
		if (temp == 7075854902445677189) {
			Item.insert(make_pair(i, make_pair("K70G2", D3DCOLOR_ARGB(255, 0xEE, 0xAD, 0x0E))));
			continue;
		}
		if (temp == 3743045072230427107) {
			Item.insert(make_pair(i, make_pair("UMU", D3DCOLOR_ARGB(255, 0xEE, 0xAD, 0x0E))));
			continue;
		}
		if (temp == 17205109951320944432) {
			Item.insert(make_pair(i, make_pair("K270", D3DCOLOR_ARGB(255, 0xEE, 0xAD, 0x0E))));
			cout << name << endl;
			continue;
		}
		if (temp == 17694966574013095680) {
			Item.insert(make_pair(i, make_pair("UEGT+J", D3DCOLOR_ARGB(255, 0xEE, 0xAD, 0x0E))));
			continue;
		}
		if (temp == 5042114762087558549) {
			Item.insert(make_pair(i, make_pair("K42", D3DCOLOR_ARGB(255, 0xFF, 0x14, 0x93))));
			continue;
		}
		if (temp == 3630983345141733269) {
			cout << name << endl;
			Item.insert(make_pair(i, make_pair("?·°¸³", D3DCOLOR_ARGB(255, 0xFF, 0x14, 0x93))));
			continue;
		}
	}
	return true;
}
std::string GetNameFromId(const int& ID)
{
	DWORD_PTR singleNameChunk = mem->RPM<DWORD_PTR>(GNames + (ID / name_truck) * 8,0x8);
	DWORD_PTR singleNamePtr = mem->RPM<DWORD_PTR>(singleNameChunk + (ID % name_truck) * 8,0x8);
	char* name;
	if ((name = (char *)mem->readSize(singleNamePtr + 0x10, 62)) != NULL) { //0x18
		std::string s = std::string(name);
		delete name;
		return s;
	}
	return std::string("NULL");
}

FCameraCacheEntry GetCameraCache()
{
	FCameraCacheEntry cce;
	cce.POV.FOV = mem->RPM<float>(Actor::camera + Offset_Fov,0x4);
	cce.POV.Location = mem->RPM<Vector3>(Actor::camera + Offset_Location, 0xC);
	cce.POV.Rotation = mem->RPM<Vector3>(Actor::camera + Offset_Rotation, 0xC);
	//cce = mem->RPM<FCameraCacheEntry>(mem->RPM<DWORD_PTR>(global::playercontroller + 0x4D8) + 0x440); // 
	return cce;
}
void UpdateAddresses()
{
	//struct tsl mytsl;
	//tsl_init(&mytsl);

	global::pUWorld = mem->RPM<DWORD_PTR>(mem->RPM<DWORD_PTR>(mem->GetBase() + 0x427F820));
	//global::pGameInstance = tsl_decrypt_prop(&mytsl, global::pUWorld + World::GameInstanceOffset);
	//global::pLocalPlayer = tsl_decrypt_prop(&mytsl, mem->RPM<DWORD_PTR>(global::pGameInstance + World::LocalPlayerOffset, 0x8));
	//global::playercontroller = tsl_decrypt_prop(&mytsl, global::pLocalPlayer + World::ControllerOffset);
	//global::level = tsl_decrypt_prop(&mytsl,global::pUWorld + 0x80);
	//Actor::enlist = tsl_decrypt_actor(&mytsl, global::level + 0x1a0);
	//tsl_finit(&mytsl);
}


float GetActorHealth(const DWORD_PTR& pAActor)
{
	return mem->RPM<float>(pAActor + Health,0x4); 
}

Vector3 GetActorPos(const DWORD_PTR& entity)
{
	DWORD_PTR rootcomp = tsl_decrypt_component(entity);
	return mem->RPM<Vector3>(rootcomp + ActorPos);
}

DWORD_PTR GetComponent(const DWORD_PTR& entity) {
	//auto temp = Actor::rootcmp.find(entity);
	//DWORD_PTR rootcomp =NULL;
	//if (temp == Actor::rootcmp.end()) {
	//	rootcomp = tsl_decrypt_component(entity + RootComp);
	//	Actor::rootcmp.insert(make_pair(entity, rootcomp));
	//}
	//else {
	//	rootcomp = temp->second;
	//}
	return tsl_decrypt_component(entity);
}

D3DXMATRIX Matrix(Vector3 rot, Vector3 origin = Vector3(0, 0, 0))
{
	float radPitch = (rot.x * float(Pi) / 180.f);
	float radYaw = (rot.y * float(Pi) / 180.f);
	float radRoll = (rot.z * float(Pi) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	D3DMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;

	return matrix;
}

Vector3 WorldToScreen(Vector3 WorldLocation, FCameraCacheEntry CameraCacheL)
{
	Vector3 Screenlocation = Vector3(0, 0, 0);

	auto POV = CameraCacheL.POV;
	Vector3 Rotation = POV.Rotation;

	D3DMATRIX tempMatrix = Matrix(Rotation); // Matrix

	Vector3 vAxisX, vAxisY, vAxisZ;

	vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	Vector3 vDelta = WorldLocation - POV.Location;
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	float FovAngle = POV.FOV;
	float ScreenCenterX = s_width/2;
	float ScreenCenterY = s_height/2;

	Screenlocation.x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * (float)Pi / 360.f)) / vTransformed.z;
	Screenlocation.y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * (float)Pi / 360.f)) / vTransformed.z;
	//if (Screenlocation.x < ScreenCenterX && Screenlocation.x > 0 && Screenlocation.y>0 && Screenlocation.y<ScreenCenterY)
	//{
	//	return true;
	//}
	return Screenlocation;
}

D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
{
	D3DMATRIX pOut;
	pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
	pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
	pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
	pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
	pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
	pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
	pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
	pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
	pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
	pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
	pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
	pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
	pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
	pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
	pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
	pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

	return pOut;
}
FTransform GetBoneIndex(DWORD_PTR mesh, int index)
{
	DWORD_PTR bonearray = mem->RPM<DWORD_PTR>(mesh + BoneArrayOffset);
	return mem->RPM<FTransform>(bonearray + (index * 0x30));
}

Vector3 GetBoneWithRotation(DWORD_PTR mesh, int id)
{
	FTransform& bone = GetBoneIndex(mesh, id);
	FTransform ComponentToWorld = mem->RPM<FTransform>(mesh + Component2World);
	D3DMATRIX Matrix;
	Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());
	return Vector3(Matrix._41, Matrix._42, Matrix._43);
}

void DrawSkeleton(const DWORD_PTR& mesh,Sex sex)
{
	Vector3 neckpos = GetBoneWithRotation(mesh, Bones::neck_01);
	Vector3 pelvispos = GetBoneWithRotation(mesh, Bones::pelvis);
	Vector3 previous(0, 0, 0);
	Vector3 current, p1, c1;
	auto temp_skeleton = sex == Sex::famale ? famale_skeleton : skeleton;
	for (auto a : temp_skeleton)
	{
		previous = Vector3(0, 0, 0);
		for (int bone : a)
		{
			current = bone == Bones::neck_01 ? neckpos : (bone == Bones::pelvis ? pelvispos : GetBoneWithRotation(mesh, bone));
			if (previous.x == 0.f)
			{
				previous = current;
				continue;
			}
			p1 = WorldToScreen(previous, global::cameracache);
			c1 = WorldToScreen(current, global::cameracache);
			DrawLine(p1.x, p1.y, c1.x, c1.y, D3DCOLOR_ARGB(255, 255, 255, 255));
			previous = current;
		}
	}
}


int isUaz(int id)
{
	for (int i = 0; i < 3; i++)
		if (id == uaz[i])
			return 1;
	return 0;
}

int isDacia(int id)
{
	for (int i = 0; i < 4; i++)
		if (id == dacia[i])
			return 1;
	return 0;
}

int isBuggy(int id)
{
	for (int i = 0; i < 7; i++)
		if (id == buggy[i])
			return 1;
	return 0;
}

int isBike(int id)
{
	for (int i = 0; i < 7; i++)
		if (id == motorbike[i])
			return 1;
	return 0;
}

int isBoat(int id)
{
	if (id == boat)
		return 1;
	return 0;
	//return id == boat;
}
int isVan(int id)
{
	for (int i = 0; i < 3; i++)
		if (id == Van[i])
			return 1;
	return 0;
	
}
int isTruck(int id)
{
	for (int i = 0; i < 10; i++)
		if (id == pK[i])
			return 1;
	return 0;

}

void DrawVehicle(const DWORD_PTR& entity, Vector3& local, const char* txt)
{
	Vector3 pos = GetActorPos(entity);
	Vector3 spos = WorldToScreen(pos, global::cameracache);
	float distance = local.Distance(pos) / 100.f;
	DWORD color = 0;
	if (distance <= 1000.f)
		color = D3DCOLOR_ARGB(255, 255, 255, 0);
	DrawString((int)spos.x, (int)spos.y, color, pFont, txt, distance);
}

//pitch,yaw,roll
Vector3 ToFRotator(Vector3& Ver)
{
	Vector3 Rot_;
	float RADPI = 180 / Pi;
	Rot_.y = atan2(Ver.y, Ver.x) * RADPI;
	Rot_.x = atan2(Ver.z, sqrt((Ver.x * Ver.x) + (Ver.y * Ver.y))) * RADPI;
	Rot_.z = 0;
	return Rot_;
}
Vector3 Clamp(Vector3 Ver)
{
	Vector3 Result = Ver;
	if (Result.x > 180)
	{
		Result.x = Result.x - 360; 
	}
	else
	{
		if (Result.x < -180)
		{
			Result.x = Result.x + 360; 
		}
	}
	if (Result.y > 180) //Í¬x
	{
		Result.y = Result.y - 360;
	}
	else
	{
		if (Result.y < -180) //Í¬x
		{
			Result.y = Result.y + 360;
		}
	}

	if (Result.x > 89)
	{
		Result.x = 89;
	}
	if (Result.x < -89)
	{
		Result.x = -89;
	}

	while (Result.y < 180)
	{
		Result.y = Result.y + 360;
	}

	while (Result.y > 180)
	{
		Result.y = Result.y - 360;
	}
	Result.z = 0;
	return Result;
}

