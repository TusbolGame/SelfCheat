#include <devioctl.h>

#define DEVICE_NAME L"\\Device\\PdX03gA3p2"         //Driver Name
#define LINK_NAME L"\\DosDevices\\lf9WAQOjp8"       //Link Name
//SymbolicLinkName：符号链接名。
//DeviceName：设备对象名。

//注：在内核态，符号链接是以“\??\”或者“\DosDevices\”开头的。而在用户模式下则是以“\\.\”开头的。
#define IOCTL_InputProcessId		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x900, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_InputBaseAddress		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x901, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_OutputlpBaseAddress	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x902, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_KReadProcessMemory	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x903, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_KWriteProcessMemory	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x904, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_InputReadOrWriteLen	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x905, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_OutputlpMemoryInfo    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x906, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_CreateThread          CTL_CODE(FILE_DEVICE_UNKNOWN, 0x907, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Read          CTL_CODE(FILE_DEVICE_UNKNOWN, 0x908, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_Write         CTL_CODE(FILE_DEVICE_UNKNOWN, 0x909, METHOD_BUFFERED, FILE_ANY_ACCESS)

ULONG64 LdrInPebOffset=0x018;		//peb.ldr
ULONG64 ModListInPebOffset=0x010;	//peb.ldr.InLoadOrderModuleList

//typedef struct _KAPC_STATE
//{
//	LIST_ENTRY ApcListHead[2];
//	PKPROCESS Process;
//	UCHAR KernelApcInProgress;
//	UCHAR KernelApcPending;
//	UCHAR UserApcPending;
//} KAPC_STATE, *PKAPC_STATE;

typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY64	InLoadOrderLinks;
	LIST_ENTRY64	InMemoryOrderLinks;
	LIST_ENTRY64	InInitializationOrderLinks;
	PVOID			DllBase;
	PVOID			EntryPoint;
	ULONG			SizeOfImage;
	UNICODE_STRING	FullDllName;
	UNICODE_STRING 	BaseDllName;
	ULONG			Flags;
	USHORT			LoadCount;
	USHORT			TlsIndex;
	PVOID			SectionPointer;
	ULONG			CheckSum;
	PVOID			LoadedImports;
	PVOID			EntryPointActivationContext;
	PVOID			PatchInformation;
	LIST_ENTRY64	ForwarderLinks;
	LIST_ENTRY64	ServiceTagLinks;
	LIST_ENTRY64	StaticLinks;
	PVOID			ContextInformation;
	ULONG64			OriginalBase;
	LARGE_INTEGER	LoadTime;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

typedef struct _MEMORY_BASIC_INFORMATION {
	PVOID BaseAddress;
	PVOID AllocationBase;
	DWORD AllocationProtect;
	SIZE_T RegionSize;
	DWORD State;
	DWORD Protect;
	DWORD Type;
} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;
typedef enum _MEMORY_INFORMATION_CLASS

{

	MemoryBasicInformation,

	MemoryWorkingSetList,

	MemorySectionName

}MEMORY_INFORMATION_CLASS;

typedef struct WPMS {
	ULONG64 address;
	ULONG32 size;
}*pWPMStruct, WPMStruct;