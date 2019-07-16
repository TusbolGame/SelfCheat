#include <ntifs.h>
#include <ntddk.h>
#include <windef.h>
#include <stdlib.h>
#include <ntdef.h>
#include "myread.h"
#include "ProtectProcess.h"
#include "head.h"
//#pragma comment(lib,"ceddk.lib")

#define DIRECTORY_TABLE_BASE			0x028
//#define ImageFileName                   0x2e0
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObj, PUNICODE_STRING pRegistryString);
NTSTATUS DispatchCreate(PDEVICE_OBJECT pDevObj, PIRP pIrp);
NTSTATUS DispatchClose(PDEVICE_OBJECT pDevObj, PIRP pIrp);
VOID DriverUnload(PDRIVER_OBJECT pDriverObj);
NTSTATUS DispatchIoctl(PDEVICE_OBJECT pDevObj, PIRP pIrp);
NTKERNELAPI NTSTATUS PsLookupProcessByProcessId(IN HANDLE ProcessId,OUT PEPROCESS *Process);
NTKERNELAPI PPEB PsGetProcessPeb(PEPROCESS Process);
NTKERNELAPI VOID NTAPI KeStackAttachProcess(PEPROCESS Process, PKAPC_STATE ApcState);
NTKERNELAPI VOID NTAPI KeUnstackDetachProcess(PKAPC_STATE ApcState);



//blackBone




//PHYSICAL_ADDRESS MmGetPhysicalAddress(PVOID BaseAddress);
//PVOID MmMapIoSpace(PHYSICAL_ADDRESS PhysicalAddress,SIZE_T NumberOfBytes,MEMORY_CACHING_TYPE CacheType);

UINT32 idGame=0;
UINT32 idHind = 0;
PEPROCESS epGame=NULL;
PEPROCESS pHindProc = NULL;
UINT64 base_addr=0;
LPVOID ModuleBase = NULL;
UINT32 rw_len=0;
pWPMStruct ReadStruct = NULL;
//PMEMORY_BASIC_INFORMATION MemoryInfo = NULL;
VOID EnumModule(PEPROCESS Process)
{
	SIZE_T Peb = 0;
	SIZE_T Ldr = 0;
	PLIST_ENTRY ModListHead = 0;
	PLIST_ENTRY Module = 0;
	ANSI_STRING AnsiString;
	KAPC_STATE ks;
	if( !MmIsAddressValid(Process) )
		return;
	Peb = (SIZE_T)PsGetProcessPeb(Process);
	if (!Peb)
		return;
	KeStackAttachProcess(Process, &ks);
	__try
	{
		Ldr = Peb + (SIZE_T)LdrInPebOffset;
		ProbeForRead((CONST PVOID)Ldr, 8, 8);
		ModListHead = (PLIST_ENTRY)(*(PULONG64)Ldr + ModListInPebOffset);
		ProbeForRead((CONST PVOID)ModListHead, 8, 8);
		Module = ModListHead->Flink;
        if(ModListHead != Module)
		{
            ModuleBase = (PVOID)(((PLDR_DATA_TABLE_ENTRY)Module)->DllBase);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DbgPrint("[EnumModule]__except (EXCEPTION_EXECUTE_HANDLER)");
	}
	KeUnstackDetachProcess(&ks);
}

PVOID GetFunctionAddr(PCWSTR FunctionName)
{
    UNICODE_STRING UniCodeFunctionName;
    RtlInitUnicodeString(&UniCodeFunctionName, FunctionName);
    return MmGetSystemRoutineAddress(&UniCodeFunctionName);
}


NTSTATUS BBExecuteInNewThread(
	IN PVOID pBaseAddress,
	IN PVOID pParam,
	IN ULONG flags,
	IN BOOLEAN wait,
	OUT PNTSTATUS pExitStatus
)
{
	HANDLE hThread = NULL;
	OBJECT_ATTRIBUTES ob = { 0 };
	NTSTATUS status;
	InitializeObjectAttributes(&ob, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);

	status = ZwCreateThreadEx(
		&hThread, THREAD_QUERY_LIMITED_INFORMATION, &ob,
		ZwCurrentProcess(), pBaseAddress, pParam, flags,
		0, 0x1000, 0x100000, NULL
	);

	// Wait for completion
	if (NT_SUCCESS(status) && wait != FALSE)
	{
		// Force 60 sec timeout
		LARGE_INTEGER timeout = { 0 };
		timeout.QuadPart = -(60ll * 10 * 1000 * 1000);

		status = ZwWaitForSingleObject(hThread, TRUE, &timeout);
		if (NT_SUCCESS(status))
		{
			THREAD_BASIC_INFORMATION info = { 0 };
			ULONG bytes = 0;

			status = ZwQueryInformationThread(hThread, ThreadBasicInformation, &info, sizeof(info), &bytes);
			if (NT_SUCCESS(status) && pExitStatus)
			{
				*pExitStatus = info.ExitStatus;
			}
			else if (!NT_SUCCESS(status))
			{
				DbgPrint("BlackBone: %s: ZwQueryInformationThread failed");
			}
		}
		else
			DbgPrint("BlackBone: %s: ZwWaitForSingleObject failed\n");
	}
	else
		DbgPrint("BlackBone: %s: ZwCreateThreadEx failed");

	if (hThread)
		ZwClose(hThread);

	return status;
}


void CreateThread(IN PVOID Address) {
	KAPC_STATE apc_state;
	NTSTATUS threadStatus;
	//DbgPrint("启动远线程。\n");
	__try
	{
		KeStackAttachProcess(epGame, &apc_state);
		//创建线程并等待退出
		threadStatus = STATUS_SUCCESS;
		BBExecuteInNewThread(Address, NULL, 4, TRUE, &threadStatus);
		KeUnstackDetachProcess(&apc_state);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		KeUnstackDetachProcess(&apc_state);
	}
}
//DbgPrint("启动远线程。\n");
//__try
//{
//	KeStackAttachProcess(hGame, &apc_state);
//	//创建线程并等待退出
//	NTSTATUS threadStatus = STATUS_SUCCESS;
//	ExecuteInNewThread(w_poReadStruct->GameAddressOffset, NULL, 4, TRUE, &threadStatus);
//	KeUnstackDetachProcess(&apc_state);
//}
//__except (EXCEPTION_EXECUTE_HANDLER)
//{
//	KeUnstackDetachProcess(&apc_state);
//	NtStatus = STATUS_ABANDONED;
//}
void KReadProcessMemory(IN PEPROCESS Process, IN PVOID Address, IN UINT32 Length, OUT PVOID Buffer)
{
    KAPC_STATE apc_state;
	PHYSICAL_ADDRESS TargetAdd;
	__try
	{
		KeStackAttachProcess(epGame, &apc_state);
		ProbeForRead((CONST PVOID)Address, Length, sizeof(CHAR));
		RtlCopyMemory(Buffer, Address, Length);
		KeUnstackDetachProcess(&apc_state);
		return;
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		KeUnstackDetachProcess(&apc_state);
	}
	//PVOID mapAdd = NULL;
	//__try
	//{
	//	KeStackAttachProcess(epGame, &apc_state);
	//	TargetAdd = MmGetPhysicalAddress(Address);
	//}
	//__except (EXCEPTION_EXECUTE_HANDLER) {
	//	KeUnstackDetachProcess(&apc_state);
	//}
	//	//typedef enum _MEMORY_CACHING_TYPE {
	//	//	MmNonCached = 0,
	//	//	MmCached = 1,
	//	//	MmWriteCombined = 2,
	//	//	MmHardwareCoherentCached = 3,
	//	//	MmNonCachedUnordered = 4,
	//	//	MmUSWCCached = 5,
	//	//	MmMaximumCacheType = 6
	//	//} MEMORY_CACHING_TYPE;
	//__try
	//{
	//	//映射到虚拟地址
	//	mapAdd = MmMapIoSpace(TargetAdd, Length, MmNonCached);
	//	if (mapAdd == NULL) {
	//		MmUnmapIoSpace(mapAdd, Length);
	//		DbgPrint("NULL");
	//		return;
	//	}
 //       ProbeForRead(mapAdd, Length, sizeof(char));
 //       RtlCopyMemory(Buffer, mapAdd, Length);
	//	MmUnmapIoSpace(mapAdd,Length);
 //       KeUnstackDetachProcess(&apc_state);
 //   }
 //   __except (EXCEPTION_EXECUTE_HANDLER)
 //   {
	//	MmUnmapIoSpace(mapAdd, Length);
 //       KeUnstackDetachProcess(&apc_state);
 //   }
} 

//0x2e0 ImageFileName

void KWriteProcessMemory(IN PEPROCESS Process, IN PVOID Address, IN UINT32 Length, IN PVOID Buffer)
{
	KAPC_STATE apc_state;
	PHYSICAL_ADDRESS TargetAdd;
	//__try
	//{
	//	KeStackAttachProcess(epGame, &apc_state);
	//	//ProbeForRead((CONST PVOID)Address, Length, sizeof(CHAR));
	//	RtlCopyMemory(Address, Buffer, Length);
	//	KeUnstackDetachProcess(&apc_state);
	//}
	//__except (EXCEPTION_EXECUTE_HANDLER) {
	//	KeUnstackDetachProcess(&apc_state);
	//}
	PVOID mapAdd = NULL;
	__try
	{
		KeStackAttachProcess(epGame, &apc_state);
		TargetAdd = MmGetPhysicalAddress(Address);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		KeUnstackDetachProcess(&apc_state);
	}
	__try
	{
		mapAdd = MmMapIoSpace(TargetAdd, Length, MmNonCached);
		if (mapAdd == NULL) {
			MmUnmapIoSpace(mapAdd, Length);
			DbgPrint("NULL");
			return;
		}
		//ProbeForWrite((CONST PVOID)mapAdd, Length, sizeof(CHAR));
		RtlCopyMemory(mapAdd, Buffer, Length);
		MmUnmapIoSpace(mapAdd, Length);
		KeUnstackDetachProcess(&apc_state);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		MmUnmapIoSpace(mapAdd, Length);
		KeUnstackDetachProcess(&apc_state);
	}
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObj, PUNICODE_STRING pRegistryString)
{
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING ustrLinkName;
	UNICODE_STRING ustrDevName;
	PDEVICE_OBJECT pDevObj;
	BBInitDynamicData(&dynData);
	pDriverObj->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
	pDriverObj->MajorFunction[IRP_MJ_CLOSE] = DispatchClose;
	pDriverObj->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchIoctl;
	pDriverObj->DriverUnload = DriverUnload;
	RtlInitUnicodeString(&ustrDevName, DEVICE_NAME);
	status = IoCreateDevice(pDriverObj, 0,&ustrDevName, FILE_DEVICE_UNKNOWN,0,FALSE,&pDevObj);
	if(!NT_SUCCESS(status))
	{
		return status;
	}
	RtlInitUnicodeString(&ustrLinkName, LINK_NAME);
	status = IoCreateSymbolicLink(&ustrLinkName, &ustrDevName);
	if(!NT_SUCCESS(status))
	{
		IoDeleteDevice(pDevObj);
		return status;
	}
	BypassCheckSign(pDriverObj);
	ObProtectProcess(1);
	return status;
}

VOID DriverUnload(PDRIVER_OBJECT pDriverObj)
{
	UNICODE_STRING strLink;
	ObProtectProcess(0);
	RtlInitUnicodeString(&strLink, LINK_NAME);
	IoDeleteSymbolicLink(&strLink);
	IoDeleteDevice(pDriverObj->DeviceObject);
}
//
NTSTATUS DispatchCreate(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DispatchClose(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DispatchIoctl(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
	NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
	PIO_STACK_LOCATION pIrpStack;
	ULONG uIoControlCode;
	PVOID pIoBuffer;
	ULONG uInSize;
	ULONG uOutSize;
	pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
	uIoControlCode = pIrpStack->Parameters.DeviceIoControl.IoControlCode;
	uInSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
	uOutSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
	switch(uIoControlCode)
	{
		case IOCTL_InputProcessId:
		{
			pIoBuffer = pIrp->AssociatedIrp.SystemBuffer;
			memcpy(&idGame,pIoBuffer,sizeof(idGame));
			DbgPrint("[x64Drv] PID: %ld",idGame);
			PsLookupProcessByProcessId((HANDLE)idGame, &epGame);
			break;
		}
		case IOCTL_OutputlpBaseAddress:
        {
			pIoBuffer = pIrp->AssociatedIrp.SystemBuffer;
            EnumModule(epGame); 
            memcpy(pIoBuffer,&ModuleBase,8);
            break;
        }
		case IOCTL_InputBaseAddress:
		{
			pIoBuffer = pIrp->AssociatedIrp.SystemBuffer;
			memcpy(&base_addr,pIoBuffer,8);		
			break;
		}
        case IOCTL_InputReadOrWriteLen:
		{
			pIoBuffer = pIrp->AssociatedIrp.SystemBuffer;
			memcpy(&rw_len,pIoBuffer,4);
			break;
		}
		//case IOCTL_KReadProcessMemory: 
		//{
		//	pIoBuffer = pIrp->AssociatedIrp.SystemBuffer;
		//	KReadProcessMemory(epGame,(PVOID)base_addr,rw_len,pIoBuffer);
		//	break;
		//}
		case IOCTL_KWriteProcessMemory: 
		{
			pIoBuffer = pIrp->AssociatedIrp.SystemBuffer;
			KWriteProcessMemory(epGame,(PVOID)base_addr,rw_len,pIoBuffer);
			break;
		}
		case IOCTL_CreateThread: {
			pIoBuffer = pIrp->AssociatedIrp.SystemBuffer;
			memcpy(&base_addr, pIoBuffer, 8);
			//DbgPrint("%x", base_addr);
			CreateThread((PVOID)base_addr);
		}
		case IOCTL_Read: {
			pIoBuffer = pIrp->AssociatedIrp.SystemBuffer;
			ReadStruct = (pWPMStruct)pIoBuffer;
			pIoBuffer = pIrp->AssociatedIrp.SystemBuffer;
			KReadProcessMemory(epGame, (PVOID)ReadStruct->address, ReadStruct->size, pIoBuffer);
			break;
		}
		//case IOCTL_Write: {
		//	pIoBuffer = pIrp->AssociatedIrp.SystemBuffer;
		//	ReadStruct = (pWPMStruct)pIoBuffer;
		//	KWriteProcessMemory(epGame, (PVOID)ReadStruct->address, ReadStruct->size, pIoBuffer);
		//	break;
		//}
	}
	status = STATUS_SUCCESS;
	if(status == STATUS_SUCCESS)
		pIrp->IoStatus.Information = uOutSize;
	else
		pIrp->IoStatus.Information = 0;
	pIrp->IoStatus.Status = status;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return status;
}
