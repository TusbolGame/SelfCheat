#pragma once


#ifndef GAME_DATA
#define GAME_DATA

#include "KReader.hpp"
#include "Matrix.hpp"
#include <string>
namespace offset{

	//0x000007FEEE0EE9A8
	uint32_t GameInstance_offset = 0x180;
	//uint32_t GameInstance_offset = 0x148;
	uint32_t LocalPlayer_offset = 0x48;
	//uint32_t LocalPlayer_offset = 0x40;
	uint32_t controller_offset = 0x40;
	//uint32_t controller_offset = 0x40;

	//uint32_t ulevel_offset = 0x38;
	uint32_t ulevel_offset = 0x178;
	//uint32_t ActorArray_offset = 0xA8;
	uint32_t ActorArray_offset = 0xB0;



	uint32_t RootComponent_offset = 0x188;


	//屏幕的显示画面只会去读取矩阵然后计算，不会用这些内存中的值，需要修改componentToWorld矩阵数据 componentToWorld+0x10到traslation平移向量
	uint32_t RelativeLocation_offset = 0x200;
	uint32_t RelativeRotation_offset = 0x20C;
	uint32_t RelativeScale3D_offset = 0x240;
	uint32_t RelativeTranslation_offset = 0x24C;
}


namespace Aim_offset {
	uint32_t ControllerRotation_offset = 0x478;
}


namespace Esp_offset {
	uint32_t camera_offset = 0x438;
	uint32_t camera_location = 0x420;
	uint32_t camera_rotation = 0x42C;
	uint32_t camera_fov = 0x438;

	uint32_t componentToWorld_offset = 0x190;
	uint32_t boneArray_offset = 0x890;
}
namespace Base_data {
	uint64_t Uworld_ptr;
	uint64_t GameInstance_ptr;
	uint64_t localplayer_ptr;
	uint64_t controller_ptr;
	uint64_t camera_ptr;
	uint64_t ulevel_ptr;
	uint64_t ActorArray_ptr;
}


namespace Name_data {
	uint64_t Gnames;
}

namespace Actor_data {
	uint64_t AcknowledgedPawn;
}


namespace Actor_offset {
	uint32_t AcknowledgedPawn_offset = 0x418;
	//uint32_t USkeletalMeshComponent_offset = 0x400;
	uint32_t USkeletalMeshComponent_offset = 0x410;
	uint32_t AnimScriptInstance_offset = 0xa08;
}



static void InitData() {
	Name_data::Gnames = mem->RPM<uint64_t>(mem->GetBase() + 0x7512B30);


	Base_data::Uworld_ptr = mem->RPM<uint64_t>(mem->GetBase()+0x764B3E0);
	Base_data::GameInstance_ptr = mem->RPM<uint64_t>(Base_data::Uworld_ptr + offset::GameInstance_offset);
	Base_data::localplayer_ptr = mem->RPM<uint64_t>(mem->RPM<uint64_t>(Base_data::GameInstance_ptr+ offset::LocalPlayer_offset));
	Base_data::controller_ptr = mem->RPM<uint64_t>(Base_data::localplayer_ptr + offset::controller_offset);
	Base_data::camera_ptr = mem->RPM<uint64_t>(Base_data::controller_ptr + Esp_offset::camera_offset);



	Base_data::ulevel_ptr = mem->RPM<uint64_t>(Base_data::Uworld_ptr + offset::ulevel_offset);
	Base_data::ActorArray_ptr = mem->RPM<uint64_t>(Base_data::ulevel_ptr + offset::ActorArray_offset);



	//Base_data::GameInstance_ptr = mem->RPM<uint64_t>(Base_data::Uworld_ptr + offset::GameInstance_offset);
	//Base_data::GameInstance_ptr = mem->RPM<uint64_t>(mem->RPM<uint64_t>(Base_data::GameInstance_ptr + offset::LocalPlayer_offset));
}



static std::string GetnameFromId(const uint32_t& id) {
	DWORD_PTR singleNameChunk = mem->RPM<DWORD_PTR>(Name_data::Gnames + (id / 0x4000) * 8, 0x8);
	DWORD_PTR singleNamePtr = mem->RPM<DWORD_PTR>(singleNameChunk + (id % 0x4000) * 8, 0x8);
	char* name;
	if ((name = (char *)mem->readSize(singleNamePtr + 0x10, 62)) != NULL) { 
		std::string s = std::string(name);
		delete name;
		return s;
	}
	return std::string("NULL");
}


void CacheName() {
	for (int32_t i = 0; i < 200000; i++) {
		std::string name = GetnameFromId(i);
		std::cout << name << std::endl;
	}

}

Vector3 GetBoneWithRotation(const uint64_t& mesh, const int& id)
{
	uint64_t bonearray = mem->RPM<uint64_t>(mesh + Esp_offset::boneArray_offset);
	FTransform bone = mem->RPM<FTransform>(bonearray + (id * 0x30));
	FTransform ComponentToWorld = mem->RPM<FTransform>(mesh + Esp_offset::componentToWorld_offset);
	D3DMATRIX Matrix;
	Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());
	return Vector3(Matrix._41, Matrix._42, Matrix._43);
}



Vector3 GetActorPos(const uint64_t& Actor) {
	return mem->RPM<Vector3>(mem->RPM<uint64_t>(Actor+offset::RootComponent_offset)+offset::RelativeLocation_offset);
}
#endif 