#include "Game.h"
//
//

namespace offset {
	uint32_t Bone_Array = 0;
	uint32_t Weapon_Processor = 0;
	uint32_t Weapon_Base = 0;
	uint32_t Weapon_Speed = 0;
	uint32_t Weapon_Index = 0;
	uint32_t AnimScriptInstance = 0;
	uint32_t Rotation_Crcp = 0;
	uint32_t ItemArray_Group = 0;
	//uint32_t 
	std::vector<uint64_t> Health_add;
}
//
void find_Health(const DWORD_PTR& myplayer) {

	if (offset::Health_add.size() > 1) {
		for (auto iter = offset::Health_add.begin(); iter != offset::Health_add.end();) {
			if (100.f > mem->RPM<float>(*iter) > 0.f) {
				logfile << "health:        " << hex << *offset::Health_add.begin() - myplayer << endl;
				iter++;
				continue;
			}
			else {
				iter = offset::Health_add.erase(iter);
			}
		}
		return;
	}
	for (int i = 0; i < 0x2000; i+=4) {
		float health = mem->RPM<float>(myplayer + i);
		if (health == 100.f) {
			offset::Health_add.emplace_back(myplayer + i);
		}
	}
}

void findTeam() {
	for (int i = 0x500; i < 0x1200; i += 4) {
		uint64_t _ptr = tsl_decrypt_component(Actor::AcknowledgedPawn + i);
		int id = tsl_decrypt_id(mem->RPM<int>(_ptr + Id_Offset));
		if (GetNameFromId(id) == "Team")
		{
			logfile << "Team: 0x" << hex<< i << endl;
			return;
		}
	}
}



//
//
//
//////class UTslAnimInstance : public UAnimInstance
//////{
//////public:
//////	class ATslCharacter*                               CharacterReference;                                       // 0x05E0(0x0008) (BlueprintVisible, BlueprintReadOnly, ZeroConstructor, IsPlainOldData)
//////	class USkeletalMeshComponent*                      CharacterMesh_CP;                                         // 0x05E8(0x0008) (BlueprintVisible, ExportObject, 
//////	struct FRotator                                    ControlRotation_CP;                                       // 0x061C(0x000C)
//////log:
//////
//////AimScriptInstance:offset 5e8 PlayerMale_A_C
//////	AimScriptInstance : offset 5f0 CharacterMesh0
//////
//////
//////	AnimScriptInstance---- -
//////	class USkeletalMeshComponent : public USkinnedMeshComponent
//////{
//////	class UAnimInstance*                               AnimScriptInstance;                                       // 0x0AF0(0x0008) (ZeroConstructor, Transient, IsPlainOldData)
//////	class UAnimInstance*                               PostProcessAnimInstance;                                  // 0x0B08(0x0008) (
//////log:
//////	Line 18526 : USkeletalMeshComp : offset b00 Char_AnimBPv3_C
//////		Line 18527 : USkeletalMeshComp : offset b08 Char_AnimBPv3_C *****
////
//
//
//
//
//
//
//





void find_itm_offset(const uint64_t& _address)//address from DecActorlist which name is DroppedItemGroup or DroppedItemInteractionComponent
{
	//0xffffffff add小于这个值
	
	for (int i = 0; i < 0x600; i += 4) {
		auto i_adc = mem->RPM<TArray>(_address + i);
		if (i_adc.count > 0 && i_adc.count <= 0xf && i_adc.addr < 0xffffffff && i_adc.addr>0x10000000) {
			logfile << "add: 0x" << hex << i_adc.addr << "    offset: 0x" << i << "    count" << i_adc.count << endl;
		}
	}

	// droploc:  
	// ofs::2e0 |-14155.5|3155.51|241.402  绝对特征
	// ofs::300 |381.531|-660.188|-531.799 相对特征  也有可能都是整数

}



void find_item_loc(const uint64_t& _address) {
	for (int i = 0x50; i < 0x700; i+=4)
	{
		Vector3 _droploc = mem->RPM<Vector3>(_address + i);
		if (
			/*	(
			(_droploc.x < -1000) &&
			(_droploc.y > 5000) &&
			(abs(_droploc.z) >= 0 && abs(_droploc.z) < 2000)
			)
			&&*/
			(
				abs(_droploc.x * 100000) == floor(abs(_droploc.x) * 100000) &&
				abs(_droploc.y * 100000) == floor(abs(_droploc.y) * 100000) &&
				abs(_droploc.z * 100000) == floor(abs(_droploc.z) * 100000)
				)
			&&
			(
				floor(abs(_droploc.x) + abs(_droploc.y) + abs(_droploc.z)) > 0 &&
				floor(abs(_droploc.x) + abs(_droploc.y) + abs(_droploc.z)) != 1 &&
				floor(abs(_droploc.x) + abs(_droploc.y) + abs(_droploc.z)) != 2 &&
				floor(abs(_droploc.x) + abs(_droploc.y) + abs(_droploc.z)) != 3
				)
			&&
			!(
				(floor(abs(_droploc.x) == 0 && floor(abs(_droploc.y)) == 0)) ||
				(floor(abs(_droploc.z) == 0 && floor(abs(_droploc.y)) == 0)) ||
				(floor(abs(_droploc.x) == 0 && floor(abs(_droploc.z)) == 0))
				)

			)
			logfile << "droploc:  ofs::" << hex << i << " |" << dec << _droploc.x << "|" << _droploc.y << "|" << _droploc.z << std::endl;

	}
}
//
void find_player_bone_offset(const uint64_t& _cur_mesh_address)
{
	uint64_t cur_mesh_address = mem->RPM<uint64_t>(_cur_mesh_address + SkeletalMeshComponent);
	for (int i_bone = 0x500; i_bone <= 0x1500; i_bone+=4)
	{
		uint64_t _tmp_bone_array = mem->RPM<uint64_t>(cur_mesh_address + i_bone);
		FTransform _1 = mem->RPM<FTransform>(_tmp_bone_array + 6 * 0x30);
		if (round(_1.scale.x) == 1 && round(_1.scale.y) == 1 && round(_1.scale.z) == 1
			&&
			abs(_1.rot.w) >0.1 && abs(_1.rot.w) < 2 &&
			abs(_1.rot.x) >0.1 && abs(_1.rot.x) < 2 &&
			abs(_1.rot.y) >0.1 && abs(_1.rot.y) < 2 &&
			abs(_1.rot.z) >0.1 && abs(_1.rot.z) < 2 &&

			abs(_1.translation.x) >0.1 && abs(_1.translation.x) < 400 &&
			abs(_1.translation.y) >0.1 && abs(_1.translation.y) < 400 &&
			abs(_1.translation.z) >0.1 && abs(_1.translation.z) < 400
			)
		{
			logfile << "bone array ofs: " << hex << i_bone << oct << " | "
				<< _1.rot.w << " | " << _1.rot.x << " | " << _1.rot.y << " | " << _1.rot.z
				<< " | " << _1.scale.x << " | " << _1.scale.y << " | " << _1.scale.z
				<< " | " << _1.translation.x << " | " << _1.translation.y << " | " << _1.translation.z << std::endl;
		}
	}
}

//暂时改成这样 因为一遍就能找到
void findActorSpeed() {
	for (int i = 0; i < 0x500; i+=4) {
		v_map[i] = 0;
	}
	uint64_t root_compt = tsl_decrypt_component(Actor::AcknowledgedPawn + RootComp);
	for (std::map<uint64_t, uint64_t>::iterator it = v_map.begin(); it != v_map.end(); ++it) {
		Vector3 _vct3 = mem->RPM<Vector3>(root_compt + it->first);
		{
			//左上
			if (_vct3.x <-10 && _vct3.y < -10 && _vct3.x > -1000 && _vct3.y > -1000)
			{
				logfile << "player v offset ---" << hex << it->first << std::endl;
				break;
			}
		}
	}
}




//void findActorSpeed() {
//	for (int i = 0; i < 0x500; i++) {
//		v_map[i] = 0;
//	}
//	uint64_t root_compt = tsl_decrypt_component(Actor::AcknowledgedPawn + RootComp);
//	for (std::map<uint64_t, uint64_t>::iterator it = v_map.begin(); it != v_map.end(); ++it) {
//		Vector3 _vct3 = mem->RPM<Vector3>(root_compt + it->first);
//		{
//			//左上
//			if (_vct3.x <-10 && _vct3.y < -10 && _vct3.x > -1000 && _vct3.y > -1000)
//			{
//				;
//			}
//			else
//			{
//				v_map[it->first] = 1;
//			}
//		}
//	}
//	int _iii = 0;
//	for (std::map<uint64_t, uint64_t>::iterator it = v_map.begin(); it != v_map.end(); ++it)
//	{
//		if (it->second == 0)
//		{
//			_iii++;
//		}
//	}
//	if (_iii == 1)
//	{
//		for (std::map<uint64_t, uint64_t>::iterator it = v_map.begin(); it != v_map.end(); ++it)
//		{
//			if (it->second == 0)
//			{
//				logfile << "player v offset ---" << hex << it->first << std::endl;
//			}
//		}
//	}
//}
//



void find_offsets()
{
	if (!Actor::AcknowledgedPawn)
		return;
	if (!offset::Weapon_Processor) {
		for (int i = 0; i < 0x3000; i += 4)
		{
			uint64_t _ptr = mem->RPM<uint64_t>(Actor::AcknowledgedPawn + i);
			int _id = tsl_decrypt_id(mem->RPM<int>(_ptr + Id_Offset));
			if (GetNameFromId(_id) == "WeaponProcessorComponent") {
				logfile << "WeaponProcessor offset : 0x" << hex << i << "   "  << std::endl;
				offset::Weapon_Processor = i;
				break;
			}
		}
	}
	if (!offset::Weapon_Processor)
		return;
	uint64_t WeaponProcessor = mem->RPM<uint64_t>(Actor::AcknowledgedPawn + offset::Weapon_Processor);

	if (!offset::Weapon_Base) {
		uint64_t CurrentWeapon = 0;
		int32_t id = 0;
		std::string name;
		for (int i = 0; i < 0x1000; i+=4) {
			CurrentWeapon = mem->RPM<DWORD_PTR>(mem->RPM<DWORD_PTR>(WeaponProcessor + i));
			id = tsl_decrypt_id(mem->RPM<int>(CurrentWeapon + Id_Offset));
			name = GetNameFromId(id);
			if (
				name == "WeapM16A4_C"
				|| name == "WeapAK47_C"
				|| name == "WeapUMP_C"
				|| name == "WeapKar98k_C"
				|| name == "WeapHK416_C"
				|| name == "WeapSCAR-L_C"
				|| name == "WeapMini14_C"
				|| name == "WeapVector_C"
				|| name == "WeapUZI_C"
				|| name == "WeapM9_C"
				) {
				logfile << hex << i << "weapon_base" << "--" << name << endl;
				offset::Weapon_Base = i;
				break;
			}
		}
	}
	if (!offset::Weapon_Base)
		return;
	uint64_t WeaponAddress = mem->RPM<uint64_t>(mem->RPM<uint64_t>(WeaponProcessor + offset::Weapon_Base));
	//
	////(ret == 715) //AKM
	////(ret == 870) //SCAR
	////(ret == 900) // M16A4
	////(ret == 880)//M416 


	////(ret == 400)//ump9 
	////(ret == 350)//UZI 
	////(ret == 940)//aug 
	//

	if (!offset::Weapon_Speed) {
		float speed;
		for (int i = 0x100; i < 0x2000; i += 4) {
			speed = mem->RPM<float>(WeaponAddress + i);
			if (speed == 715 || speed == 870 || speed == 900 || speed == 880) {
				logfile << hex << i << "weapon speed: " << endl;
				offset::Weapon_Speed = i;
				break;
			}
		}
	}

	if (!offset::Weapon_Index) {
		for (int _i = 0; _i <= 0x50; _i += 4)
		{
			int32_t cur_weapon_idx = mem->RPM<int32_t>(WeaponProcessor + offset::Weapon_Base + _i);
			if (cur_weapon_idx >= 1 && cur_weapon_idx <= 2) {
				logfile << "weapon cntofs: " << hex << "0x" << _i << " value : " << cur_weapon_idx << std::endl;
				offset::Weapon_Index = _i;
				break;
			}
		}
	}


	//////////////cpcr

	uint64_t my_mesh = mem->RPM<uint64_t>(Actor::AcknowledgedPawn + SkeletalMeshComponent);
	if (!offset::AnimScriptInstance) {
		int count = 0;
		for (int i = 0; i < 0x2000; i += 4)
		{
			uint64_t _ptr = mem->RPM<uint64_t>(my_mesh + i);
			int _id = tsl_decrypt_id(mem->RPM<int>(_ptr + Id_Offset));
			std::string name = GetNameFromId(_id);
			if (name == "Char_AnimBPv3_C")
			{
				//取第二个
				if (!count) {
					count++;
					continue;
				}
				logfile << "AnimScriptInstance : 0x" << hex << i << " " << name << std::endl;
				offset::AnimScriptInstance = i;
				break;
			}
		}
	}
	if (!offset::AnimScriptInstance)
		return;
	uint64_t my_anim = mem->RPM<uint64_t>(my_mesh + offset::AnimScriptInstance);


	if (!offset::Rotation_Crcp) {
			//for (int i = 0; i <= 0x1000; i++) //面朝150刻度 150-90 = 60 crcp.y
			//{
			//	auto crcp = mem->RPM<Vector3>(my_anim + i);
			//	if(crcp.x >=0 && crcp.x <= 360 &&
			//		crcp.y >= 55 && crcp.y <= 65 &&
			//		crcp.z >= 0 && crcp.z <= 360 
			//		&& round(abs(crcp.x) + abs(crcp.y)+abs(crcp.z))
			//		)
			//	logfile << "crcp ofs : 0x" << hex << i << oct 
			//		<<" | " << crcp.x << " | " << crcp.y << " | " <<crcp.z << std::endl;
			//}


		for (int i = 0; i < 0x1000; i += 4)
		{
			uint64_t _ptr = mem->RPM<uint64_t>(my_anim + i);
			int _id = tsl_decrypt_id(mem->RPM<int>(_ptr + Id_Offset));
			std::string name = GetNameFromId(_id);
			if (name == "CharacterMesh0")
			{
				logfile << "OFFSET_CR_CP : 0x" << hex << i+0x34 << " " << name << std::endl;
			}
		}
	}
}



//
//
////CRCP
//	//for (int i = 0; i <= 0x1000; i++) //面朝150刻度 150-90 = 60 crcp.y
//	//{
//	//	vect3 crcp = read_type<vect3>(my_anim + i);
//	//	if(crcp.x >=0 && crcp.x <= 360 &&
//	//		crcp.y >= 55 && crcp.y <= 65 &&
//	//		crcp.z >= 0 && crcp.z <= 360 
//	//		&& round(myabs(crcp.x) + myabs(crcp.y)+ myabs(crcp.z))
//	//		)
//	//	logfile << "crcp ofs : 0x" << hex << i << oct 
//	//		<<" | " << crcp.x << " | " << crcp.y << " | " <<crcp.z << std::endl;
//	//}