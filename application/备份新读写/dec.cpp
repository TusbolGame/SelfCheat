#include "Game.h"






int tsl_init(struct tsl *tsl) {
	tsl->func = (decrypt_func)VirtualAlloc(NULL, 0x400, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	return tsl->func == NULL ? 0 : 1;
}

void tsl_finit(struct tsl *tsl) {
	if (tsl->func != NULL) {

		VirtualFree(tsl->func, 0, MEM_RELEASE);
		tsl->func = NULL;
	}
}

// ida

#define BYTEn(x, n) (*((BYTE *)&(x) + n))
#define WORDn(x, n) (*((WORD *)&(x) + n))
#define DWORDn(x, n) (*((DWORD *)&(x) + n))

#define IDA_LOBYTE(x) BYTEn(x, 0)
#define IDA_LOWORD(x) WORDn(x, 0)
#define IDA_LODWORD(x) DWORDn(x, 0)
#define IDA_HIBYTE(x) BYTEn(x, 1)
#define IDA_HIWORD(x) WORDn(x, 1)
#define IDA_HIDWORD(x) DWORDn(x, 1)

#define BYTE1(x) BYTEn(x, 1)
#define BYTE2(x) BYTEn(x, 2)
#define WORD1(x) WORDn(x, 1)
#define DWORD1(x) DWORDn(x, 1)


#define READ(src, dest, size) mem->ReadS(src, dest, size)
#define READ32(addr) mem->RPM<uint32_t>(addr,0x4)
#define READ64(addr) mem->RPM<uint64_t>(addr,0x8)

#define TABLE 0x40506A0


// rotate

static uint8_t rol1(uint8_t x, unsigned int count) {

	count %= 8;
	return (x << count) | (x >> (8 - count));
}

static uint16_t rol2(uint16_t x, unsigned int count) {

	count %= 16;
	return (x << count) | (x >> (16 - count));
}

static uint32_t rol4(uint32_t x, unsigned int count) {

	count %= 32;
	return (x << count) | (x >> (32 - count));
}

static uint64_t rol8(uint64_t x, unsigned int count) {

	count %= 64;
	return (x << count) | (x >> (64 - count));
}

static uint8_t ror1(uint8_t x, unsigned int count) {

	count %= 8;
	return (x << (8 - count)) | (x >> count);
}

static uint16_t ror2(uint16_t x, unsigned int count) {

	count %= 16;
	return (x << (16 - count)) | (x >> count);
}

static uint32_t ror4(uint32_t x, unsigned int count) {

	count %= 32;
	return (x << (32 - count)) | (x >> count);
}

static uint64_t ror8(uint64_t x, unsigned int count) {

	count %= 64;
	return (x << (64 - count)) | (x >> count);
}


// credit: DirtyFrank

struct rel_addr {
	uint32_t offset;
	uint32_t addr;
};

static int find_call(const uint8_t *buf, uint32_t size, struct rel_addr *ret) {
	uint32_t offset = 0;
	while (offset < (size - 5)) {
		if (buf[offset] == 0xe8) {
			uint32_t addr = *(uint32_t *)(buf + (offset + 1));
			if (addr < 0x1FFFF) {
				ret->offset = offset + 5;
				ret->addr = addr;
				return 1;
			}
		}
		offset++;
	}
	return 0;
}

static uint32_t get_func_len(const uint8_t *buf, uint32_t size, uint8_t start, uint32_t end) {
	if (*buf == start) {
		uint32_t offset = 0;
		while (offset < (size - sizeof(end))) {
			if (*(uint32_t *)(buf + offset) == end) {
				return offset;
			}
			offset++;
		}
	}
	return 0;
}

static uint64_t decrypt(struct tsl *tsl, uint64_t func, uint64_t arg) {
	uint8_t buf_0x100[0x100];
	if (!READ(func, buf_0x100, 0x100)) {
		return 0;
	}
	struct rel_addr rel_addr;
	if (!find_call(buf_0x100, 0x100, &rel_addr)) {
		return 0;
	}
	uint64_t abs_addr = func + (rel_addr.offset + rel_addr.addr);
	uint8_t buf_0x20[0x20];
	if (!READ(abs_addr, buf_0x20, 0x20)) {
		return 0;
	}
	uint32_t len = get_func_len(buf_0x20, 0x20, 0x48, 0xccccccc3);
	if (!len || len > 0xf) {
		return 0;
	}
	uint32_t temp = rel_addr.offset - 5;
	memcpy(tsl->func, buf_0x100, temp);
	memcpy((char *)tsl->func + temp, buf_0x20, len);
	memcpy((char *)tsl->func + (temp + len), buf_0x100 + rel_addr.offset, 0x100 - rel_addr.offset);
	uint64_t ret = tsl->func(arg);
	memset(tsl->func, 0, 0x200);
	return ret;
}



BYTE deshellcode[40] = {
	0x48,0x83,0xec,0x28,  //sub rsp,0x28
	0x48,0xb9,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //mov rcx,0x0
	0xe8,0x00,0x00,0x00,0x00, //call 
	0x48,0x89,0x05,0x05,0x00,0x00,0x00,
	0x48,0x83,0xc4,0x28,
	0xc3,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
void Set_shellcode(DWORD_PTR func)
{
	DWORD_PTR shell = mem->GetBase() + 0x4DE71D0;
	*(DWORD*)(deshellcode + 15) = func - (shell + 14) - 5;
	mem->WPM(shell, deshellcode, sizeof(deshellcode));
}
BYTE clean2[40] = { NULL };

DWORD_PTR Decrypt(DWORD_PTR func, DWORD_PTR toDec) // 函数地址与函数参数
{
	Set_shellcode(func);
	DWORD_PTR shell = mem->GetBase() + 0x4DE71D0;
	mem->WPM(shell + 6, &toDec, sizeof(DWORD_PTR));
	mem->MCreateThread(shell);
	DWORD_PTR value = mem->RPM<DWORD_PTR>(shell + 31, sizeof(DWORD_PTR));
	mem->WPM(shell, clean2, sizeof(deshellcode));
	if (value)
	{
		return value;
	}
	return 0;
}


/****************************************~IDA dec~*****************************************************/
uint64_t tsl_decrypt_actor(struct tsl  *mytsl,uint64_t actor) {
	struct uint128_t xmm;
	if (!READ(actor, &xmm, 16)) {
		return 0;
	}
	uint32_t key = (uint32_t)xmm.low;
	uint16_t v13 =rol2(HIWORD(key), 8);
	int v14 = (v13 + 60209) ^ (unsigned __int16)~(~(WORD)key - 33);
	uint64_t func = READ64(mem->GetBase() + TABLE + 0x8 * (((unsigned __int8)~(~((v13 + 49) ^ ~(~(BYTE)key - 33)) + 87) ^ ((unsigned __int8)(BYTE1(v14) - 57) + 94)) % 128));
	return ror8(decrypt(mytsl, func, (xmm.high + key)), 109);
}




uint64_t tsl_decrypt_prop(struct tsl  *mytsl,uint64_t prop) {
	struct uint128_t xmm;
	if (!READ(prop, &xmm, 16)) {
		return 0;
	}
	uint32_t key = (uint32_t)xmm.low;
	int v7 = (unsigned __int16)(key + 98) ^ ((unsigned __int16)(IDA_HIWORD(key) + 26) + 13954);
	uint64_t func = READ64(mem->GetBase() + TABLE + 0x8 * (((unsigned __int8)(((key + 98) ^ (BYTE2(key) - 100)) + 114) ^ ((unsigned __int8)(BYTE1(v7) + 110) + 124))% 128));
	return ror8(decrypt(mytsl, func, (xmm.high ^ key)), 122);
}




uint32_t tsl_decrypt_id(const int& id) {
	int v4 = rol4(id ^ 0x13B95120, 0xE);
	int decid = (v4 << 16)  ^ 0x9C21A82F ^ v4;
	return decid;
}


uint64_t tsl_decrypt_component(struct tsl  *mytsl, uint64_t root) {
	struct uint128_t xmm;
	if (!READ(root, &xmm, 16)) {
		return 0;
	}
	uint32_t key = (uint32_t)xmm.low;
	uint64_t func = READ64(mem->GetBase() + TABLE + 0x8 * ((((unsigned __int8)(((unsigned __int16)(key ^ (WORD1(key) - 32 + 8288)) >> 8)
		- 96)
		+ 64) ^ (unsigned __int8)(key ^ (((unsigned int)key >> 16) + 64)))
		% 128));
	return ror8(decrypt(mytsl, func, (xmm.high - key)), -32);
}

uint64_t tsl_decrypt_component(const uint64_t& root) {
	uint64_t ret = 0;
	uint64_t v3 = mem->RPM<DWORD_PTR>(root);
	int v4 = ror4(v3 ^ 0x593303BD, 16);
	IDA_LODWORD(v3) = ror4(IDA_HIDWORD(v3) ^ 0xBE62C6E8, 8);
	int v5 = ror4((v4 - 193682228) ^ 0xF43303BD, 16);
	IDA_LODWORD(v3) = ror4((v3 - 213126324) ^ 0x5962C6E8, 8);
	IDA_LODWORD(ret) = v5 ^ 0xF474A4CC;
	IDA_HIDWORD(ret) = v3 ^ 0xCB40CB4;
	return ret;
}




uint64_t tsl_decrypt_actor(uint64_t actor) {
	struct uint128_t xmm;
	if (!READ(actor, &xmm, 16)) {
		return 0;
	}
	uint32_t key = (uint32_t)xmm.low;
	uint32_t v5 = (unsigned __int16)(key - 78) ^ ((unsigned __int16)(HIWORD(key) + 42) + 42706);
	uint64_t func = READ64(mem->GetBase() + TABLE + 0x8 * (((unsigned __int8)(((key - 78) ^ (BYTE2(key) - 4)) + 66) ^ ((unsigned __int8)(BYTE1(v5) + 98) + 220)) % 128));
	return ror8(Decrypt( func, (xmm.high ^ key)), -118);
}


uint64_t tsl_decrypt_prop(uint64_t prop) {
	struct uint128_t xmm;
	if (!READ(prop, &xmm, 16)) {
		return 0;
	}
	uint32_t key = (uint32_t)xmm.low;
	uint32_t v11 = (unsigned __int16)(key + 50) ^ ((key >> 16) + 9810);
	uint64_t func = READ64(mem->GetBase() + TABLE + 0x8 * (((unsigned __int8)(((key + 50) ^ ((key >> 16) + 82)) - 62) ^ ((unsigned __int8)(BYTE1(v11) - 30) + 220)) % 128));
	return ror8(Decrypt(func, (xmm.high ^ key)), 10);

}






//**************************************************************my dec//
//BYTE GetGnameptr[] = {
//	0x48,0x83,0xec,0x28,
//	0xe8,0x00,0x00,0x00,0x00,
//	0x48,0x89,0x05,0x05,0x00,0x00,0x00,
//	0x48,0x83,0xc4,0x28,
//	0xc3,
//	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
//};
//BYTE CleanGname[29] = { NULL };
//static uint64_t get_nameptr() {
//	uint64_t ThreadAdd = mem->GetBase()+ 0x4B5DA70;
//	*(uint32_t *)(GetGnameptr + 5) = (mem->GetBase()+0xB9D5A0) - (ThreadAdd + 4) - 5;
//	mem->WPM(ThreadAdd, GetGnameptr, sizeof(GetGnameptr));
//	mem->MCreateThread(ThreadAdd);
//	uint64_t value = mem->RPM<uint64_t>(ThreadAdd + 21, sizeof(uint64_t));
//	mem->WPM(ThreadAdd, CleanGname, sizeof(deshellcode));
//	if (value)
//	{
//		return value;
//	}
//	return 0;
//}
//
//BYTE DecGnamesShell[] = {
//	0x48,0x81,0xEC,0x80,0x00,0x00,0x00,
//	0x41,0xB9,0x00,0x00,0x00,0x00,
//	0x41,0x8B,0xC1,0xC1,0xE8,0x10,0x66,0x83,0xE8,0x60,0x0F,0xB7,0xC8,0x81,0xC1,0x20,0x60,0x00,0x00,0x41,0x0F,0xB7,0xC1,0x33,0xC8,0x8B,0xC1,0xC1,0xF8,0x08,0x2C,0x20,0x0F,0xB6,0xD0,0x81,0xC2,0xC0,0x00,0x00,0x00,0x0F,0xB6,0xC1,0x33,0xD0,0x81,0xE2,0x7F,0x00,0x00,0x80,0x7D,0x07,0xFF,0xCA,0x83,0xCA,0x80,0xFF,0xC2,
//	0x48,0xB9,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//	0x48,0x63,0xD2,0x49,0x2B,0xC9,
//	0x48,0x89,0x0D,0x0F,0x00,0x00,0x00, //mov rcx
//	0x48,0x89,0x15,0x10,0x00,0x00,0x00, //mov rdx
//	0x48,0x81,0xC4,0x80,0x00,0x00,0x00,
//	0xC3,
//	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //rcx
//	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //rdx
//};
//uint64_t tsl_decrypt_gnames(PDecStruct decPtrst) {
//	struct uint128_t xmm;
//	if (!READ(get_nameptr(), &xmm, 16)) {
//		return 0;
//	}
//	*(uint32_t *)((uint8_t *)(decPtrst->decGnames) + 9) = (uint32_t)xmm.low;
//	*(uint64_t *)((uint8_t *)(decPtrst->decGnames) + 76) = xmm.high;
//	decPtrst->decGnames();
//	uint64_t param = *(uint64_t *)((unsigned char *)(decPtrst->decGnames) + 112);
//	uint64_t funcadd = *(uint64_t *)((unsigned char *)(decPtrst->decGnames) + 120);
//	funcadd = READ64(mem->GetBase() + TABLE + funcadd * 8);
//	return ror8(Decrypt(funcadd, param), 160);
//}
//
//uint64_t tsl_decrypt_world(uint64_t world) {
//	return 0;
//}
//
//BYTE DecActorShell[] = {
//	0x48,0x81,0xEC,0x80,0x00,0x00,0x00,
//	0x48,0x81,0xC4,0x80,0x00,0x00,0x00,
//	0xC3,
//	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
//};
//uint64_t tsl_decrypt_actor(PDecStruct decPtrst, uint64_t actor) {
//	struct uint128_t xmm;
//	if (!READ(actor, &xmm, 16)) {
//		return 0;
//	}
//	*(uint32_t *)((uint8_t *)(decPtrst->decActor) + 9) = (uint32_t)xmm.low;
//	*(uint64_t *)((uint8_t *)(decPtrst->decActor) + 101) = xmm.high;
//	decPtrst->decProp();
//	uint64_t param = *(uint64_t *)((unsigned char *)(decPtrst->decActor) + 146);
//	uint64_t funcadd = *(uint64_t *)((unsigned char *)(decPtrst->decActor) + 154);
//	funcadd = READ64(mem->GetBase() + TABLE + funcadd * 8);
//	return ror8(Decrypt(funcadd, param), 129);
//}
//
//BYTE DecPropShell[] = { 
//	0x48,0x81,0xEC,0x80,0x00,0x00,0x00,
//    0x48,0x81,0xC4,0x80,0x00,0x00,0x00,
//    0xC3,
//	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
//};
//uint64_t tsl_decrypt_prop(PDecStruct decPtrst,uint64_t prop) {
//	struct uint128_t xmm;
//	if (!READ(prop, &xmm, 16)) {
//		return 0;
//	}
//	*(uint32_t *)((uint8_t *)(decPtrst->decProp) + 9) = (uint32_t)xmm.low;
//	*(uint64_t *)((uint8_t *)(decPtrst->decProp) + 101) = xmm.high;
//	decPtrst->decProp();
//	uint64_t param = *(uint64_t *)((unsigned char *)(decPtrst->decProp) + 146);
//	uint64_t funcadd = *(uint64_t *)((unsigned char *)(decPtrst->decProp) + 154);
//	funcadd = READ64(mem->GetBase() + TABLE + funcadd * 8);
//	return ror8(Decrypt(funcadd, param), 129);
//}
//
//bool tsl_init(PDecStruct decPtrst) {
//	decPtrst->decActor = (decryptPTR)VirtualAlloc(NULL, 0x100, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
//	memcpy(decPtrst->decActor, DecActorShell, sizeof(DecActorShell));
//	decPtrst->decProp = (decryptPTR)VirtualAlloc(NULL, 0x100, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
//	memcpy(decPtrst->decProp, DecPropShell, sizeof(DecPropShell));
//	decPtrst->decGnames = (decryptPTR)VirtualAlloc(NULL, 0x100, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
//	memcpy(decPtrst->decGnames, DecGnamesShell, sizeof(DecGnamesShell));
//	if (decPtrst->decActor == NULL || decPtrst->decProp == NULL)
//		return false;
//	return true;
//}
//
//void tsl_finit(PDecStruct decPtrst) {
//	if (decPtrst->decActor != NULL&&decPtrst->decProp != NULL) {
//		VirtualFree(decPtrst->decActor, 0, MEM_RELEASE);
//		VirtualFree(decPtrst->decProp, 0, MEM_RELEASE);
//	}
//}
//
