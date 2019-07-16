#include "NativeEnums.h"

//#ifdef __cplusplus
//extern "C++"
//{
//	char _RTL_CONSTANT_STRING_type_check(const char *s);
//	char _RTL_CONSTANT_STRING_type_check(const WCHAR *s);
//	// __typeof would be desirable here instead of sizeof.
//	template <size_t N> class _RTL_CONSTANT_STRING_remove_const_template_class;
//	template <> class _RTL_CONSTANT_STRING_remove_const_template_class<sizeof(char)> { public: typedef  char T; };
//	template <> class _RTL_CONSTANT_STRING_remove_const_template_class<sizeof(WCHAR)> { public: typedef WCHAR T; };
//#define _RTL_CONSTANT_STRING_remove_const_macro(s) \
//    (const_cast<_RTL_CONSTANT_STRING_remove_const_template_class<sizeof((s)[0])>::T*>(s))
//}
//#else
//char _RTL_CONSTANT_STRING_type_check(const void *s);
//#define _RTL_CONSTANT_STRING_remove_const_macro(s) (s)
//#endif
//#define RTL_CONSTANT_STRING(s) \
//{ \
//    sizeof( s ) - sizeof( (s)[0] ), \
//    sizeof( s ) / sizeof(_RTL_CONSTANT_STRING_type_check(s)), \
//    _RTL_CONSTANT_STRING_remove_const_macro(s) \
//}

PVOID g_KernelBase = NULL;
ULONG g_KernelSize = 0; 
#define BB_POOL_TAG 'enoB'
#define IMAGE_SCN_MEM_NOT_PAGED              0x08000000  // Section is not pageable.
#define IMAGE_SCN_MEM_SHARED                 0x10000000  // Section is shareable.
#define IMAGE_SCN_MEM_EXECUTE                0x20000000  // Section is executable.
#define IMAGE_SCN_MEM_DISCARDABLE            0x02000000  // Section can be discarded.




typedef NTSTATUS(NTAPI* fnNtCreateThreadEx)
(
	OUT PHANDLE hThread,
	IN ACCESS_MASK DesiredAccess,
	IN PVOID ObjectAttributes,
	IN HANDLE ProcessHandle,
	IN PVOID lpStartAddress,
	IN PVOID lpParameter,
	IN ULONG Flags,
	IN SIZE_T StackZeroBits,
	IN SIZE_T SizeOfStackCommit,
	IN SIZE_T SizeOfStackReserve,
	OUT PVOID lpBytesBuffer
	);


NTSYSAPI
PIMAGE_NT_HEADERS
NTAPI
RtlImageNtHeader(PVOID Base);

typedef struct _NT_PROC_THREAD_ATTRIBUTE_ENTRY
{
	ULONG Attribute;    // PROC_THREAD_ATTRIBUTE_XXX
	SIZE_T Size;
	ULONG_PTR Value;
	ULONG Unknown;
} NT_PROC_THREAD_ATTRIBUTE_ENTRY, *NT_PPROC_THREAD_ATTRIBUTE_ENTRY;

typedef struct _NT_PROC_THREAD_ATTRIBUTE_LIST
{
	ULONG Length;
	NT_PROC_THREAD_ATTRIBUTE_ENTRY Entry[1];
} NT_PROC_THREAD_ATTRIBUTE_LIST, *PNT_PROC_THREAD_ATTRIBUTE_LIST;


typedef enum _WinVer
{
	WINVER_7 = 0x0610,
	WINVER_7_SP1 = 0x0611,
	WINVER_8 = 0x0620,
	WINVER_81 = 0x0630,
	WINVER_10 = 0x0A00,
	WINVER_10_RS1 = 0x0A01, // Anniversary update
	WINVER_10_RS2 = 0x0A02, // Creators update
	WINVER_10_RS3 = 0x0A03, // Fall creators update
} WinVer;

typedef struct _DYNAMIC_DATA
{
	WinVer  ver;            // OS version
	ULONG   buildNo;        // OS build revision
	BOOLEAN correctBuild;   // OS kernel build number is correct and supported

	ULONG KExecOpt;         // KPROCESS::ExecuteOptions 
	ULONG Protection;       // EPROCESS::Protection
	ULONG EProcessFlags2;   // EPROCESS::Flags2
	ULONG ObjTable;         // EPROCESS::ObjectTable
	ULONG VadRoot;          // EPROCESS::VadRoot
	ULONG NtProtectIndex;   // NtProtectVirtualMemory SSDT index
	ULONG NtCreateThdIndex; // NtCreateThreadEx SSDT index
	ULONG NtTermThdIndex;   // NtTerminateThread SSDT index
	ULONG PrevMode;         // KTHREAD::PreviousMode
	ULONG ExitStatus;       // ETHREAD::ExitStatus
	ULONG MiAllocPage;      // MiAllocateDriverPage offset
	ULONG ExRemoveTable;    // Ex(p)RemoveHandleTable offset

	ULONG_PTR DYN_PDE_BASE; // Win10 AU+ relocated PDE base VA
	ULONG_PTR DYN_PTE_BASE; // Win10 AU+ relocated PTE base VA
} DYNAMIC_DATA, *PDYNAMIC_DATA;


DYNAMIC_DATA dynData;

typedef struct _SYSTEM_SERVICE_DESCRIPTOR_TABLE
{
	PULONG_PTR ServiceTableBase;
	PULONG ServiceCounterTableBase;
	ULONG_PTR NumberOfServices;
	PUCHAR ParamTableBase;
} SYSTEM_SERVICE_DESCRIPTOR_TABLE, *PSYSTEM_SERVICE_DESCRIPTOR_TABLE;


PSYSTEM_SERVICE_DESCRIPTOR_TABLE g_SSDT = NULL;

typedef struct _RTL_PROCESS_MODULE_INFORMATION
{
	HANDLE Section;         // Not filled in
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	UCHAR  FullPathName[MAXIMUM_FILENAME_LENGTH];
} RTL_PROCESS_MODULE_INFORMATION, *PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES
{
	ULONG NumberOfModules;
	RTL_PROCESS_MODULE_INFORMATION Modules[1];
} RTL_PROCESS_MODULES, *PRTL_PROCESS_MODULES;


typedef struct _THREAD_BASIC_INFORMATION
{
	NTSTATUS ExitStatus;
	PVOID TebBaseAddress;
	CLIENT_ID ClientId;
	ULONG_PTR AffinityMask;
	LONG Priority;
	LONG BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;


#define IMAGE_SIZEOF_SHORT_NAME              8
typedef struct _IMAGE_SECTION_HEADER {
	BYTE    Name[IMAGE_SIZEOF_SHORT_NAME];
	union {
		DWORD   PhysicalAddress;
		DWORD   VirtualSize;
	} Misc;
	DWORD   VirtualAddress;
	DWORD   SizeOfRawData;
	DWORD   PointerToRawData;
	DWORD   PointerToRelocations;
	DWORD   PointerToLinenumbers;
	WORD    NumberOfRelocations;
	WORD    NumberOfLinenumbers;
	DWORD   Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

typedef struct _IMAGE_FILE_HEADER {
	WORD    Machine;
	WORD    NumberOfSections;
	DWORD   TimeDateStamp;
	DWORD   PointerToSymbolTable;
	DWORD   NumberOfSymbols;
	WORD    SizeOfOptionalHeader;
	WORD    Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

typedef struct _IMAGE_DATA_DIRECTORY {
	DWORD   VirtualAddress;
	DWORD   Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES    16

typedef struct _IMAGE_OPTIONAL_HEADER64 {
	WORD        Magic;
	BYTE        MajorLinkerVersion;
	BYTE        MinorLinkerVersion;
	DWORD       SizeOfCode;
	DWORD       SizeOfInitializedData;
	DWORD       SizeOfUninitializedData;
	DWORD       AddressOfEntryPoint;
	DWORD       BaseOfCode;
	ULONGLONG   ImageBase;
	DWORD       SectionAlignment;
	DWORD       FileAlignment;
	WORD        MajorOperatingSystemVersion;
	WORD        MinorOperatingSystemVersion;
	WORD        MajorImageVersion;
	WORD        MinorImageVersion;
	WORD        MajorSubsystemVersion;
	WORD        MinorSubsystemVersion;
	DWORD       Win32VersionValue;
	DWORD       SizeOfImage;
	DWORD       SizeOfHeaders;
	DWORD       CheckSum;
	WORD        Subsystem;
	WORD        DllCharacteristics;
	ULONGLONG   SizeOfStackReserve;
	ULONGLONG   SizeOfStackCommit;
	ULONGLONG   SizeOfHeapReserve;
	ULONGLONG   SizeOfHeapCommit;
	DWORD       LoaderFlags;
	DWORD       NumberOfRvaAndSizes;
	IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER64, *PIMAGE_OPTIONAL_HEADER64;

typedef struct _IMAGE_NT_HEADERS64
{
	ULONG Signature;
	struct _IMAGE_FILE_HEADER FileHeader;
	struct _IMAGE_OPTIONAL_HEADER64 OptionalHeader;
} IMAGE_NT_HEADERS64, *PIMAGE_NT_HEADERS64;


/// <summary>
/// PDE/PTE dynamic code offsets
/// </summary>
typedef struct _TABLE_OFFSETS
{
	int PDE;
	int PTE;
} TABLE_OFFSETS, *PTABLE_OFFSETS;

/// <summary>
/// Pre/Post 'meltdown' patch offsets
/// </summary>
typedef struct _TABLE_OFFSETS_MELT
{
	// selector[0] - offsets for builds before 'meltdown' patch
	// selector[1] - offsets for builds after  'meltdown' patch
	TABLE_OFFSETS selector[2];
} TABLE_OFFSETS_MELT, *PTABLE_OFFSETS_MELT;

NTSYSAPI
NTSTATUS
NTAPI
ZwQuerySystemInformation(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
	OUT PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength OPTIONAL
);

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryInformationThread(
	IN HANDLE ThreadHandle,
	IN THREADINFOCLASS ThreadInformationClass,
	OUT PVOID ThreadInformation,
	IN ULONG ThreadInformationLength,
	OUT PULONG ReturnLength OPTIONAL
);

PVOID GetKernelBase(OUT PULONG pSize)
{
	NTSTATUS status = STATUS_SUCCESS;
	ULONG bytes = 0;
	PRTL_PROCESS_MODULES pMods = NULL;
	PVOID checkPtr = NULL;
	UNICODE_STRING routineName;

	// Already found
	if (g_KernelBase != NULL)
	{
		if (pSize)
			*pSize = g_KernelSize;
		return g_KernelBase;
	}

	RtlInitUnicodeString(&routineName, L"NtOpenFile");

	checkPtr = MmGetSystemRoutineAddress(&routineName);
	if (checkPtr == NULL)
		return NULL;

	// Protect from UserMode AV
	status = ZwQuerySystemInformation(SystemModuleInformation, 0, bytes, &bytes);
	if (bytes == 0)
	{
		//DPRINT("BlackBone: %s: Invalid SystemModuleInformation size\n", __FUNCTION__);
		return NULL;
	}

	pMods = (PRTL_PROCESS_MODULES)ExAllocatePoolWithTag(NonPagedPool, bytes, BB_POOL_TAG);
	RtlZeroMemory(pMods, bytes);

	status = ZwQuerySystemInformation(SystemModuleInformation, pMods, bytes, &bytes);

	if (NT_SUCCESS(status))
	{
		PRTL_PROCESS_MODULE_INFORMATION pMod = pMods->Modules;
		ULONG i;
		for (i=0; i < pMods->NumberOfModules; i++)
		{
			// System routine is inside module
			if (checkPtr >= pMod[i].ImageBase &&
				checkPtr < (PVOID)((PUCHAR)pMod[i].ImageBase + pMod[i].ImageSize))
			{
				g_KernelBase = pMod[i].ImageBase;
				g_KernelSize = pMod[i].ImageSize;
				if (pSize)
					*pSize = g_KernelSize;
				break;
			}
		}
	}

	if (pMods)
		ExFreePoolWithTag(pMods, BB_POOL_TAG);

	return g_KernelBase;
}

NTSTATUS BBSearchPattern(IN PCUCHAR pattern, IN UCHAR wildcard, IN ULONG_PTR len, IN const VOID* base, IN ULONG_PTR size, OUT PVOID* ppFound)
{
	ULONG_PTR i;
	ULONG_PTR j;
	//ASSERT(ppFound != NULL && pattern != NULL && base != NULL);
	if (ppFound == NULL || pattern == NULL || base == NULL)
		return STATUS_INVALID_PARAMETER;
	for (i = 0; i < size - len; i++)
	{
		BOOLEAN found = TRUE;
		for (j = 0; j < len; j++)
		{
			if (pattern[j] != wildcard && pattern[j] != ((PCUCHAR)base)[i + j])
			{
				found = FALSE;
				break;
			}
		}

		if (found != FALSE)
		{
			*ppFound = (PUCHAR)base + i;
			return STATUS_SUCCESS;
		}
	}

	return STATUS_NOT_FOUND;
}

PSYSTEM_SERVICE_DESCRIPTOR_TABLE GetSSDTBase()
{
	PIMAGE_NT_HEADERS pHdr;
	PIMAGE_SECTION_HEADER pFirstSec;
	PIMAGE_SECTION_HEADER pSec;
	PUCHAR ntosBase = GetKernelBase(NULL);
	// Already found
	if (g_SSDT != NULL)
		return g_SSDT;

	if (!ntosBase)
		return NULL;

	pHdr = RtlImageNtHeader(ntosBase);
	pFirstSec = (PIMAGE_SECTION_HEADER)(pHdr + 1);
	for (pSec = pFirstSec; pSec < pFirstSec + pHdr->FileHeader.NumberOfSections; pSec++)
	{
		// Non-paged, non-discardable, readable sections
		// Probably still not fool-proof enough...
		if (pSec->Characteristics & IMAGE_SCN_MEM_NOT_PAGED &&
			pSec->Characteristics & IMAGE_SCN_MEM_EXECUTE &&
			!(pSec->Characteristics & IMAGE_SCN_MEM_DISCARDABLE) &&
			(*(PULONG)pSec->Name != 'TINI') &&
			(*(PULONG)pSec->Name != 'EGAP'))
		{
			PVOID pFound = NULL;

			// KiSystemServiceRepeat pattern
			UCHAR pattern[] = "\x4c\x8d\x15\xcc\xcc\xcc\xcc\x4c\x8d\x1d\xcc\xcc\xcc\xcc\xf7";
			NTSTATUS status = BBSearchPattern(pattern, 0xCC, sizeof(pattern) - 1, ntosBase + pSec->VirtualAddress, pSec->Misc.VirtualSize, &pFound);
			if (NT_SUCCESS(status))
			{
				g_SSDT = (PSYSTEM_SERVICE_DESCRIPTOR_TABLE)((PUCHAR)pFound + *(PULONG)((PUCHAR)pFound + 3) + 7);
				//DPRINT( "BlackBone: %s: KeSystemServiceDescriptorTable = 0x%p\n", __FUNCTION__, g_SSDT );
				return g_SSDT;
			}
		}
	}

	return NULL;
}

PVOID GetSSDTEntry(IN ULONG index)
{
	ULONG size = 0;
	PSYSTEM_SERVICE_DESCRIPTOR_TABLE pSSDT = GetSSDTBase();
	PVOID pBase = GetKernelBase(&size);

	if (pSSDT && pBase)
	{
		// Index range check
		if (index > pSSDT->NumberOfServices)
			return NULL;

		return (PUCHAR)pSSDT->ServiceTableBase + (((PLONG)pSSDT->ServiceTableBase)[index] >> 4);
	}

	return NULL;
}

NTSTATUS
NTAPI
ZwCreateThreadEx(
	OUT PHANDLE hThread,
	IN ACCESS_MASK DesiredAccess,
	IN PVOID ObjectAttributes,
	IN HANDLE ProcessHandle,
	IN PVOID lpStartAddress,
	IN PVOID lpParameter,
	IN ULONG Flags,
	IN SIZE_T StackZeroBits,
	IN SIZE_T SizeOfStackCommit,
	IN SIZE_T SizeOfStackReserve,
	IN PNT_PROC_THREAD_ATTRIBUTE_LIST AttributeList
)
{
	NTSTATUS status = STATUS_SUCCESS;

	fnNtCreateThreadEx NtCreateThreadEx = (fnNtCreateThreadEx)(ULONG_PTR)GetSSDTEntry(dynData.NtCreateThdIndex);
	if (NtCreateThreadEx)
	{
		//
		// If previous mode is UserMode, addresses passed into ZwCreateThreadEx must be in user-mode space
		// Switching to KernelMode allows usage of kernel-mode addresses
		//
		PUCHAR pPrevMode = (PUCHAR)PsGetCurrentThread() + dynData.PrevMode;
		UCHAR prevMode = *pPrevMode;
		*pPrevMode = KernelMode;

		status = NtCreateThreadEx(
			hThread, DesiredAccess, ObjectAttributes,
			ProcessHandle, lpStartAddress, lpParameter,
			Flags, StackZeroBits, SizeOfStackCommit,
			SizeOfStackReserve, AttributeList
		);

		*pPrevMode = prevMode;
	}
	else
		status = STATUS_NOT_FOUND;

	return status;
}

NTSTATUS BBGetBuildNO(OUT PULONG pBuildNo)
{
	NTSTATUS status;
	UNICODE_STRING strRegKey = RTL_CONSTANT_STRING(L"\\Registry\\Machine\\Software\\Microsoft\\Windows NT\\CurrentVersion");
	UNICODE_STRING strRegValue = RTL_CONSTANT_STRING(L"BuildLabEx");
	UNICODE_STRING strRegValue10 = RTL_CONSTANT_STRING(L"UBR");
	UNICODE_STRING strVerVal = { 0 };
	HANDLE hKey = NULL;
	OBJECT_ATTRIBUTES keyAttr = { 0 };
	//ASSERT(pBuildNo != NULL);
	if (pBuildNo == NULL)
		return STATUS_INVALID_PARAMETER;
	status = STATUS_SUCCESS;
	InitializeObjectAttributes(&keyAttr, &strRegKey, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);

	status = ZwOpenKey(&hKey, KEY_READ, &keyAttr);
	if (NT_SUCCESS(status))
	{
		PKEY_VALUE_FULL_INFORMATION pValueInfo =  ExAllocatePoolWithTag(PagedPool, PAGE_SIZE, BB_POOL_TAG);
		ULONG bytes = 0;

		if (pValueInfo)
		{
			// Try query UBR value
			status = ZwQueryValueKey(hKey, &strRegValue10, KeyValueFullInformation, pValueInfo, PAGE_SIZE, &bytes);
			if (NT_SUCCESS(status))
			{
				*pBuildNo = *(PULONG)((PUCHAR)pValueInfo + pValueInfo->DataOffset);
				goto skip1;
			}

			status = ZwQueryValueKey(hKey, &strRegValue, KeyValueFullInformation, pValueInfo, PAGE_SIZE, &bytes);
			if (NT_SUCCESS(status))
			{
				PWCHAR pData = (PWCHAR)((PUCHAR)pValueInfo->Name + pValueInfo->NameLength);
				ULONG i;
				for (i = 0; i < pValueInfo->DataLength; i++)
				{
					if (pData[i] == L'.')
					{
						ULONG j;
						for (j = i + 1; j < pValueInfo->DataLength; j++)
						{
							if (pData[j] == L'.')
							{
								strVerVal.Buffer = &pData[i] + 1;
								strVerVal.Length = strVerVal.MaximumLength = (USHORT)((j - i) * sizeof(WCHAR));
								status = RtlUnicodeStringToInteger(&strVerVal, 10, pBuildNo);

								goto skip1;
							}
						}
					}
				}

			skip1:;
			}

			ExFreePoolWithTag(pValueInfo, BB_POOL_TAG);
		}
		else
			status = STATUS_NO_MEMORY;

		ZwClose(hKey);
	}
	else
		DbgPrint("BlackBone: %s: ZwOpenKey failed");

	return status;

}

NTSTATUS BBScanSection(IN PCCHAR section, IN PCUCHAR pattern, IN UCHAR wildcard, IN ULONG_PTR len, OUT PVOID* ppFound)
{
	PVOID base;
	PIMAGE_NT_HEADERS64 pHdr;
	PIMAGE_SECTION_HEADER pFirstSection;
	PIMAGE_SECTION_HEADER pSection;
	ASSERT(ppFound != NULL);
	if (ppFound == NULL)
		return STATUS_INVALID_PARAMETER;
	base = GetKernelBase(NULL);
	if (!base)
		return STATUS_NOT_FOUND;

	pHdr = RtlImageNtHeader(base);
	if (!pHdr)
		return STATUS_INVALID_IMAGE_FORMAT;

	pFirstSection = (PIMAGE_SECTION_HEADER)(pHdr + 1);
	for (pSection = pFirstSection; pSection < pFirstSection + pHdr->FileHeader.NumberOfSections; pSection++)
	{
		ANSI_STRING s1, s2;
		RtlInitAnsiString(&s1, section);
		RtlInitAnsiString(&s2, (PCCHAR)pSection->Name);
		if (RtlCompareString(&s1, &s2, TRUE) == 0)
		{
			PVOID ptr = NULL;
			NTSTATUS status = BBSearchPattern(pattern, wildcard, len, (PUCHAR)base + pSection->VirtualAddress, pSection->Misc.VirtualSize, &ptr);
			if (NT_SUCCESS(status))
				*(PULONG)ppFound = (ULONG)((PUCHAR)ptr - (PUCHAR)base);

			return status;
		}
	}

	return STATUS_NOT_FOUND;
}

NTSTATUS BBLocatePageTables(IN OUT PDYNAMIC_DATA pData)
{
	const int index = pData->ver & 0xFF;
	const TABLE_OFFSETS_MELT offsets[4] =
	{
		{ 0, 0, 0, 0 },             // No updates
		{ 0x49, 0x56, 0x52, 0x5F }, // WINVER_10_AU
		{ 0x43, 0x50, 0x4B, 0x58 }, // WINVER_10_CU
		{ 0x41, 0x4E, 0x4B, 0x58 }  // WINVER_10_FC
	};
	const ULONG patchThreshold[] =
	{
		0,      // No updates
		2007,   // WINVER_10_AU
		850,    // WINVER_10_CU
		192     // WINVER_10_FC
	};
	UNICODE_STRING uName = RTL_CONSTANT_STRING(L"MmGetPhysicalAddress");
	PUCHAR pMmGetPhysicalAddress = MmGetSystemRoutineAddress(&uName);
	ASSERT(pData->ver >= WINVER_10_RS1);
	if (pMmGetPhysicalAddress)
	{
		PUCHAR pMiGetPhysicalAddress = *(PLONG)(pMmGetPhysicalAddress + 0xE + 1) + pMmGetPhysicalAddress + 0xE + 5;

		// Meltdown fix check
		const int melt = (pData->buildNo >= patchThreshold[index]) ? 1 : 0;

		pData->DYN_PDE_BASE = *(PULONG_PTR)(pMiGetPhysicalAddress + offsets[index].selector[melt].PDE + 2);
		pData->DYN_PTE_BASE = *(PULONG_PTR)(pMiGetPhysicalAddress + offsets[index].selector[melt].PTE + 2);

		DbgPrint("BlackBone: PDE_BASE: %p, PTE_BASE: %p\n", pData->DYN_PDE_BASE, pData->DYN_PTE_BASE);
		return STATUS_SUCCESS;
	}

	DbgPrint("BlackBone: PDE_BASE/PTE_BASE not found \n");  //DPRINT
	return STATUS_NOT_FOUND;
}
NTSTATUS BBInitDynamicData(IN OUT PDYNAMIC_DATA pData)
{
	NTSTATUS status = STATUS_SUCCESS;
	RTL_OSVERSIONINFOEXW verInfo = { 0 };

	if (pData == NULL)
		return STATUS_INVALID_ADDRESS;

	RtlZeroMemory(pData, sizeof(DYNAMIC_DATA));
	pData->DYN_PDE_BASE = PDE_BASE;
	pData->DYN_PTE_BASE = PTE_BASE;

	verInfo.dwOSVersionInfoSize = sizeof(verInfo);
	status = RtlGetVersion((PRTL_OSVERSIONINFOW)&verInfo);

	if (status == STATUS_SUCCESS)
	{
		ULONG ver_short = (verInfo.dwMajorVersion << 8) | (verInfo.dwMinorVersion << 4) | verInfo.wServicePackMajor;
		pData->ver = (WinVer)ver_short;

		// Get kernel build number
		status = BBGetBuildNO(&pData->buildNo);

		// Validate current driver version
		pData->correctBuild = TRUE;
#if defined(_WIN7_)
		if (ver_short != WINVER_7 && ver_short != WINVER_7_SP1)
			return STATUS_NOT_SUPPORTED;
#elif defined(_WIN8_)
		if (ver_short != WINVER_8)
			return STATUS_NOT_SUPPORTED;
#elif defined (_WIN81_)
		if (ver_short != WINVER_81)
			return STATUS_NOT_SUPPORTED;
#elif defined (_WIN10_)
		if (ver_short < WINVER_10 || WINVER_10_RS3 < ver_short)
			return STATUS_NOT_SUPPORTED;
#endif

		DbgPrint(
			"BlackBone: OS version %d.%d.%d.%d.%d - 0x%x\n",
			verInfo.dwMajorVersion,
			verInfo.dwMinorVersion,
			verInfo.dwBuildNumber,
			verInfo.wServicePackMajor,
			pData->buildNo,
			ver_short
		);

		switch (ver_short)
		{
			// Windows 7
			// Windows 7 SP1
		case WINVER_7:
		case WINVER_7_SP1:
			pData->KExecOpt = 0x0D2;
			pData->Protection = 0x43C;  // Bitfield, bit index - 0xB
			pData->ObjTable = 0x200;
			pData->VadRoot = 0x448;
			pData->NtProtectIndex = 0x04D;
			pData->NtCreateThdIndex = 0x0A5;
			pData->NtTermThdIndex = 0x50;
			pData->PrevMode = 0x1F6;
			pData->ExitStatus = 0x380;
			pData->MiAllocPage = (ver_short == WINVER_7_SP1) ? 0 : 0;
			if (ver_short == WINVER_7_SP1)
			{
				if (NT_SUCCESS(BBScanSection("PAGE", (PCUCHAR)"\x48\x8D\x56\x20\x48\x8B\x42\x08", 0xCC, 8, (PVOID)&pData->ExRemoveTable)))
					pData->ExRemoveTable -= 0x36;
			}
			else
				pData->ExRemoveTable = 0x32D404;
			break;

			// Windows 8
		case WINVER_8:
			pData->KExecOpt = 0x1B7;
			pData->Protection = 0x648;
			pData->ObjTable = 0x408;
			pData->VadRoot = 0x590;
			pData->NtProtectIndex = 0x04E;
			pData->NtCreateThdIndex = 0x0AF;
			pData->NtTermThdIndex = 0x51;
			pData->PrevMode = 0x232;
			pData->ExitStatus = 0x450;
			pData->MiAllocPage = 0x3AF374;
			pData->ExRemoveTable = 0x487518;
			break;

			// Windows 8.1
		case WINVER_81:
			pData->KExecOpt = 0x1B7;
			pData->Protection = 0x67A;
			pData->EProcessFlags2 = 0x2F8;
			pData->ObjTable = 0x408;
			pData->VadRoot = 0x5D8;
			pData->NtCreateThdIndex = 0xB0;
			pData->NtTermThdIndex = 0x52;
			pData->PrevMode = 0x232;
			pData->ExitStatus = 0x6D8;
			pData->MiAllocPage = 0;
			pData->ExRemoveTable = 0x432A88; // 0x38E320;
			if (NT_SUCCESS(BBScanSection("PAGE", (PCUCHAR)"\x48\x8D\x7D\x18\x48\x8B", 0xCC, 6, (PVOID)&pData->ExRemoveTable)))
				pData->ExRemoveTable -= 0x5E;
			break;

			// Windows 10, build 16299/15063/14393/10586
		case WINVER_10:
			if (verInfo.dwBuildNumber == 10586)
			{
				pData->KExecOpt = 0x1BF;
				pData->Protection = 0x6B2;
				pData->EProcessFlags2 = 0x300;
				pData->ObjTable = 0x418;
				pData->VadRoot = 0x610;
				pData->NtCreateThdIndex = 0xB4;
				pData->NtTermThdIndex = 0x53;
				pData->PrevMode = 0x232;
				pData->ExitStatus = 0x6E0;
				pData->MiAllocPage = 0;
				if (NT_SUCCESS(BBScanSection("PAGE", (PCUCHAR)"\x48\x8D\x7D\x18\x48\x8B", 0xCC, 6, (PVOID)&pData->ExRemoveTable)))
					pData->ExRemoveTable -= 0x5C;
				break;
			}
			else if (verInfo.dwBuildNumber == 14393)
			{
				pData->ver = WINVER_10_RS1;
				pData->KExecOpt = 0x1BF;
				pData->Protection = 0x6C2;
				pData->EProcessFlags2 = 0x300;
				pData->ObjTable = 0x418;
				pData->VadRoot = 0x620;
				pData->NtCreateThdIndex = 0xB6;
				pData->NtTermThdIndex = 0x53;
				pData->PrevMode = 0x232;
				pData->ExitStatus = 0x6F0;
				pData->MiAllocPage = 0;
				if (NT_SUCCESS(BBScanSection("PAGE", (PCUCHAR)"\x48\x8D\x7D\x18\x48\x8B", 0xCC, 6, (PVOID)&pData->ExRemoveTable)))
					pData->ExRemoveTable -= 0x60;

				status = BBLocatePageTables(pData);
				break;
			}
			else if (verInfo.dwBuildNumber == 15063)
			{
				pData->ver = WINVER_10_RS2;
				pData->KExecOpt = 0x1BF;
				pData->Protection = 0x6CA;
				pData->EProcessFlags2 = 0x300;
				pData->ObjTable = 0x418;
				pData->VadRoot = 0x628;
				pData->NtCreateThdIndex = 0xB9;
				pData->NtTermThdIndex = 0x53;
				pData->PrevMode = 0x232;
				pData->ExitStatus = 0x6F8;
				pData->MiAllocPage = 0;
				if (NT_SUCCESS(BBScanSection("PAGE", (PCUCHAR)"\x48\x8B\x47\x20\x48\x83\xC7\x18", 0xCC, 8, (PVOID)&pData->ExRemoveTable)))
					pData->ExRemoveTable -= 0x34;

				status = BBLocatePageTables(pData);
				break;
			}
			else if (verInfo.dwBuildNumber == 16299)
			{
				pData->ver = WINVER_10_RS3;
				pData->KExecOpt = 0x1BF;
				pData->Protection = 0x6CA;
				pData->EProcessFlags2 = 0x828;    // MitigationFlags offset
				pData->ObjTable = 0x418;
				pData->VadRoot = 0x628;
				pData->NtCreateThdIndex = 0xBA;
				pData->NtTermThdIndex = 0x53;
				pData->PrevMode = 0x232;
				pData->ExitStatus = 0x700;
				pData->MiAllocPage = 0;
				if (NT_SUCCESS(BBScanSection("PAGE", (PCUCHAR)"\x48\x83\xC7\x18\x48\x8B\x17", 0xCC, 7, (PVOID)&pData->ExRemoveTable)))
					pData->ExRemoveTable -= 0x34;

				status = BBLocatePageTables(pData);
				break;
			}
			else
			{
				return STATUS_NOT_SUPPORTED;
			}
		default:
			break;
		}

		if (pData->ExRemoveTable != 0)
			pData->correctBuild = TRUE;

		DbgPrint(
			"BlackBone: Dynamic search status: SSDT - %s, ExRemoveTable - %s\n",
			GetSSDTBase() != NULL ? "SUCCESS" : "FAIL",
			pData->ExRemoveTable != 0 ? "SUCCESS" : "FAIL"
		);

		//return (pData->VadRoot != 0 ? status : STATUS_INVALID_KERNEL_INFO_VERSION);
		return status;
	}

	return status;
}