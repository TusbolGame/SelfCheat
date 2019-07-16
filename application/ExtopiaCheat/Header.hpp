//NTSTATUS Mapped_Memory_Addr(ULONG LowAddr, ULONG HigAddr, PULONG Addr_Arry, int Mapped_len)
//{
//	NTSTATUS Status = STATUS_UNSUCCESSFUL;
//	PHYSICAL_ADDRESS Physical_address;
//	PULONG Page_dri_base;
//	RtlZeroMemory(&Physical_address, sizeof(PHYSICAL_ADDRESS));
//	if (LowAddr == 0)
//	{
//		return Status;
//	}
//
//
//	Physical_address.u.LowPart = ProcessCr3Low; //ָ��ý��̵�ҳĿ¼���ַ
//	Physical_address.u.HighPart = ProcessCr3Hig; //ָ��ý��̵ĳ��ռ�(Hyper Space)��ҳĿ¼���ַ
//	DbgBreakPoint();
//
//	//cr3 = Page Map Level 4
//	//win10 1803����ϵͳ����ֱ���õ�cr3ҳ��������ڴ��ַ�ˣ�ͨ��mdlӳ��
//	PULONG Page_dri_base = (PULONG)MmMapIoSpace(Physical_address, Mapped_len, TRUE);
//
//	if (ARGUMENT_PRESENT(Page_dri_base) && MmIsAddressValid(Page_dri_base))
//	{
//		for (ULONG i = 0; i < PAGE_DIR_MAX; i++)
//		{
//			Addr_Arry[i] = Page_dri_base[i];
//			if (i % 8 == 0)
//			{
//				DbgPrint("\n");
//			}
//
//			DbgPrint("%08X ", Addr_Arry[i]);
//
//		}
//		DbgPrint("\n");
//		Status = STATUS_SUCCESS;
//		MmUnmapIoSpace(Page_dri_base, Mapped_len);
//		return Status;
//	}
//
//	return Status;
//}
//
//
//
//NTSTATUS OpenProcess(HANDLE ProcessPid)
//{
//
//	PEPROCESS  ProcessInfo = NULL;
//	PKPROCESS   ProccessMemoryInfo = NULL;
//	NTSTATUS Status = STATUS_UNSUCCESSFUL;
//	ProcessCr3Low = 0;
//	ProcessCr3Hig = 0;
//	if (ProcessPid < 0)
//		return Status;
//	Status = PsLookupProcessByProcessId(ProcessPid, &ProcessInfo);
//
//	if (!NT_SUCCESS(Status))
//	{
//		DbgPrint("GetPidCr3Error:%X", Status);
//		return Status;
//	}
//
//	if (ARGUMENT_PRESENT(ProcessInfo) && MmIsAddressValid(ProcessInfo))
//	{
//		ProccessMemoryInfo = ProcessInfo + 0X0;
//		Status = STATUS_SUCCESS;
//		ProcessCr3Low = ProccessMemoryInfo->DirectoryTableBase[0];
//		ProcessCr3Hig = ProccessMemoryInfo->DirectoryTableBase[1];
//		DbgPrint("CR3 =%X%X\n", ProcessCr3Hig, ProcessCr3Low);
//		return Status;
//	}
//
//	return Status;
//}
//LONGLONG Get_Last_digit(ULONG Addr)
//{
//	LONGLONG Tmp = Addr;
//	Tmp &= 0x000000000F;
//	Tmp *= 0x100000000;
//	return Tmp;
//}
//
//ULONGLONG translate_Linear_address(int Linearaddress)
//{
//	NTSTATUS Status = STATUS_UNSUCCESSFUL;
//	VIRTUAL_ADDRESS virtualAddress = { 0 };
//	ULONG tmp_arry[PAGE_DIR_MAX] = { 0 };
//	ULONG32 Tmp = 0;
//
//
//	virtualAddress.ulVirtualAddress = Linearaddress;
//
//	if (ProcessCr3Low == 0)
//	{
//		DbgPrint("OpenProcessFail\n");
//		return Status;
//	}
//
//	DbgPrint("CR3 =%X%X,offset = %X,pt = %X,pdt = %X,pdpt = %X,plm4 = %X\n", ProcessCr3Hig, ProcessCr3Low, virtualAddress.stVirtualAddress.offset, virtualAddress.stVirtualAddress.PT, virtualAddress.stVirtualAddress.PDT, virtualAddress.stVirtualAddress.PDPT, virtualAddress.stVirtualAddress.PLM4);
//
//	Status = Mapped_Memory_Addr(ProcessCr3Low, ProcessCr3Hig, tmp_arry, MAPING_MAX_LEN);
//
//	if (!NT_SUCCESS(Status))
//	{
//		DbgPrint("PLM4_Addr Mapped Fail\n");
//		return Status;
//	}
//
//	/* ULONG PLM4_Addr = tmp_arry[virtualAddress.stVirtualAddress.PLM4];
//	Tmp = tmp_arry[virtualAddress.stVirtualAddress.PLM4 + 1];
//	Tmp &= 0x000000000F;
//	PLM4_Addr &= 0x000000FFFFFFF000;
//	DbgPrint("PLM4_Addr = %X\n", PLM4_Addr);
//
//
//
//
//	ULONG PDPT_Addr = PLM4_Addr + virtualAddress.stVirtualAddress.PDPT * 0x8;
//	Status = Mapped_Memory_Addr(PDPT_Addr, Tmp,tmp_arry, MAPING_MAX_LEN);
//	if (!NT_SUCCESS(Status))
//	{
//	DbgPrint("PDPT_Addr Mapped Fail\n");
//	return Status;
//	}
//
//
//
//	PDPT_Addr = tmp_arry[0];
//	Tmp = tmp_arry[1];
//	Tmp &= 0x000000000F;
//	PDPT_Addr &= 0x000000FFFFFFF000;
//	DbgPrint("PDPT_Addr = %X\n", PDPT_Addr);
//
//	ULONG PDT_Addr = PDPT_Addr + virtualAddress.stVirtualAddress.PDT * 0x8;
//	Status = Mapped_Memory_Addr(PDT_Addr, Tmp,tmp_arry, MAPING_MAX_LEN);
//
//	if (!NT_SUCCESS(Status))
//	{
//	DbgPrint("PDT_Addr Mapped Fail\n");
//	return Status;
//	}
//
//
//	PDT_Addr = tmp_arry[0];
//	Tmp = tmp_arry[1];
//	Tmp &= 0x000000000F;
//	PDT_Addr &= 0x000000FFFFFFF000;
//
//	DbgPrint("PDT_Addr = %X\n", PDT_Addr);
//
//	ULONG PT_Addr = PDT_Addr + virtualAddress.stVirtualAddress.PT * 0x8;
//	Status = Mapped_Memory_Addr(PT_Addr, Tmp,tmp_arry, MAPING_MAX_LEN);
//
//	if (!NT_SUCCESS(Status))
//	{
//	DbgPrint("PT_Addr Mapped Fail\n");
//	return Status;
//	}
//	PT_Addr = tmp_arry[0];
//	Tmp = tmp_arry[1];
//	Tmp &= 0x000000000F;
//	PT_Addr &= 0x000000FFFFFFF000;
//	DbgPrint("PT_Addr = %X\n", PT_Addr);
//
//
//	ULONGLONG Physucal_Addr = PT_Addr + virtualAddress.stVirtualAddress.offset;
//	Physucal_Addr = Physucal_Addr | Get_Last_digit(tmp_arry[1]);
//	return Physucal_Addr;
//
//	/*PHYSICAL_ADDRESS Physical_address;
//	DbgPrint("Physucal_Addr: %p\n", Physucal_Addr);
//	RtlZeroMemory(&Physical_address, sizeof(PHYSICAL_ADDRESS));
//	Physical_address.u.LowPart = Physucal_Addr;
//	Physical_address.u.HighPart = Tmp;
//	char *Buffer = (char *)MmMapIoSpace(Physical_address, sizeof(PHYSICAL_ADDRESS), TRUE);
//	DbgPrint("Data:%s\n", Buffer);*/
//	return 0L;
//}
//
//
//
//int    _tmain(int    argc, _TCHAR*    argv[])
//{
//	BOOL    IsError = 0;//����һ�������ʶ
//	DWORD    LastError = 0;//����һ���������
//	MEMORY_BASIC_INFORMATION    lpBuffer = { 0 };//����һ��VirtualQueryEx�Ĳ����Ľṹ
//	SIZE_T    RegionSize = 0;
//	PVOID    Address = 0;
//	HANDLE    hProcess = NULL;
//	HANDLE    hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//	PROCESSENTRY32    pe;
//	pe.dwSize = sizeof(PROCESSENTRY32);
//	if (!Process32First(hSnapShot, &pe))
//	{
//		return    0;
//	}
//	while (Process32Next(hSnapShot, &pe))
//	{
//		if (::StringCompareW(pe.szExeFile, TEXT("Taskmgr.exe"), FALSE))
//		{
//			hProcess = ::OpenProcessByProcessId(PROCESS_ALL_ACCESS, 0, pe.th32ProcessID);
//			if (hProcess == NULL)
//			{
//				LastError = GetLastError();
//				if (LastError == 5)
//				{
//					if (!::IsAdmin())
//					{
//						if (MessageBox(NULL, TEXT("Ȩ�޲��㣡������Ȩ�ޣ�\r\n�Ƿ�����Ȩ�ޣ�"), TEXT("Ȩ�޲��㣡"), 48 + MB_YESNO))
//						{
//							CloseHandle(hSnapShot);
//							::ShellExecuteExAs(argv[0], argv[0], ::GetCommandLine());
//							return    0;
//						}
//					}
//				}
//
//				IsError = TRUE;
//				goto    Error;
//			}
//			else
//			{
//				goto    Next;
//			}
//		}
//	}
//	MessageBox(NULL, TEXT("�Ҳ�����������������������������!"), TEXT("����"), 48);
//	CloseHandle(hSnapShot);
//	return    0;
//Next:
//	RegionSize = sizeof(PVOID);
//	if (::MmAllocateVirtualMemoryEx(hProcess, &Address, 0, &RegionSize, MEM_COMMIT, PAGE_READWRITE))//��������������������ڴ�
//	{
//		if (::MmVirtualQueryEx(hProcess, Address, &lpBuffer, RegionSize) != 0)//��ѯ������ڴ�����
//		{
//			MessageBox(NULL, TEXT("�������гɹ�!"), TEXT("����"), 0);
//		}
//		::MmFreeVirtualMemoryEx(hProcess, &Address, &RegionSize, MEM_RELEASE);//�ͷ�������ڴ�����
//	}
//	else
//	{
//		LastError = GetLastError();
//		IsError = TRUE;
//	}
//	//    ::MmVirtualQueryEx(
//	::CloseObjectHandle(hProcess);
//Error:
//	CloseHandle(hSnapShot);
//	if (IsError)
//	{
//		LPCTSTR    ErrorStr = ::GetLastErrorMessageW(LastError);//���ش������ĸ�ʽ���ַ�
//		MessageBox(NULL, ErrorStr, TEXT("����"), 16);
//		::FreeMemory(ErrorStr);//�ͷ��ڴ棬ֻ���ͷ����ҵĳ�����д�Ĵ���������ڴ棬���������ͷű�Ĵ���������ڴ�
//		ErrorStr = NULL;
//	}
//	return    0;
//}
//
//
//
//
//SIZE_T
//WINAPI
//MmVirtualQueryEx(
//	__in          HANDLE  hProcess,
//	__in_opt  LPCVOID  lpAddress,
//	__out_bcount_part(dwLength, return)  PMEMORY_BASIC_INFORMATION  lpBuffer,
//	__in          SIZE_T  dwLength
//	)
//{
//	SIZE_T  ReturnSize = 0;
//	NTSTATUS  Status = 0;
//	if (!::ProbeForWrite(lpBuffer, sizeof(MEMORY_BASIC_INFORMATION)))//���в�����飬���ӳ����ȶ��ԣ����Ǳ�Ҫ������ȥ��
//		return  FALSE;
//	Status = ZwQueryVirtualMemory(hProcess, (PVOID)lpAddress, MemoryBasicInformation, lpBuffer, sizeof(MEMORY_BASIC_INFORMATION), &ReturnSize);//��������������ZwQueryVirtualMemory
//	if (!NT_SUCCESS(Status))//����һ��NT�����ú�NT_SUCCESS�жϴ������ͣ����NT_SUCCESS�жϳɹ���������true�����ϵͳִ������������ϵͳִ�г���
//	{
//		//ϵͳִ�г���
//		::BaseSetLastNTError(Status);//�Զ��庯�������ܽ�NT����ת��Ϊϵͳ���󣬲��ҵ���SetLastError�����ô���
//		return  FALSE;//����һ��FALSE��Ҳ����0��
//	}
//	return  ReturnSize;//���óɹ��������ڴ���С�����ﻹӦ���ȵ���һ��::BaseSetLastNTError(NOERROR);�ģ�������ʾϵͳ����û�д����Ҹ����ˣ�
//					   /*return  VirtualQueryEx(
//					   hProcess,
//					   lpAddress,
//					   lpBuffer,
//					   dwLength);*/
//}
//
//
//1��R3Ӧ�ò��£�DebugActiveProcess ���ص�����ʱ��������һ��Զ�̶ϵ㣬�����Զ�̶ϵ���ʵ���һ�٣�������ʱ�ᴥ�������Լ�⡣ΨһҪ���ľ����ƹ�����
//
//����NtDebugActiveProcess���ϲ㺯����DbgUiDebugActiveProcess�ķ������룺
//_DbgUiDebugActiveProcess@4:
//00:  mov         edi, edi
//7760FC4C  push        ebp
//7760FC4D  mov         ebp, esp
//7760FC4F  mov         eax, dword ptr fs : [00000018h]
//7760FC55  push        esi
//7760FC56  push        dword ptr[eax + 0F24h]
//7760FC5C  push        dword ptr[ebp + 8]
//7760FC5F  call        _NtDebugActiveProcess@8 (775909A4h)
//7760FC64  mov         esi, eax        // ԭʼ���ص�ַ
//7760FC66  test        esi, esi
//7760FC68  jl          _DbgUiDebugActiveProcess@4+36h(7760FC80h)
//7760FC6A  push        dword ptr[ebp + 8]
//7760FC6D  call        _DbgUiIssueRemoteBreakin@4 (7760FC03h)
//7760FC72  mov         esi, eax
//7760FC74  test        esi, esi
//7760FC76  jge         _DbgUiDebugActiveProcess@4+36h(7760FC80h)
//7760FC78  push        dword ptr[ebp + 8]
//7760FC7B  call        _DbgUiStopDebugging@4 (7760FB88h)
//7760FC80  mov         eax, esi
//7760FC82  pop         esi        // +0x1E��ķ��ص�ַ
//7760FC83  pop         ebp
//7760FC84  ret         4
//
//������64λ WINDOW7 ����� NtDebugActiveProcess �Ĺ��Ӻ����ľ�����룬�ƹ�Զ�̶ϵ�ķ������ڷ��ص�ַ�ϼ�ƫ��0x1E
//
////
//// NtDebugActiveProcess
////
//NTSTATUS WINAPI DbgNtDebugActiveProcess64(
//	__in HANDLE ProcessHandle,
//	__in HANDLE DebugObjectHandle)
//{
//	NTSTATUS Status = TrueNtDebugActiveProcess(ProcessHandle, DebugObjectHandle);
//
//
//	KdPrint((_T("NtDebugActiveProcess ����ֵ: 0x%08x!\r\n"), Status));
//
//	//
//	// �ƹ�Ӧ�ò��Զ�̶ϵ㣬��ȫ���һ�ٰ���
//	//
//	if (NT_SUCCESS(Status))
//	{
//		LPVOID ReturnAddress = (LPSTR)&ProcessHandle - sizeof(DWORD);
//		*(LPDWORD)ReturnAddress += 0x1E;
//	}
//	return Status;
//}
//
//2�����������������������ô��R0���Ĳ��µ��Զ˿�������Ǿ��������Ϸ������ѡ�񡣽��̵ĵ��Զ˿ڱ�����һ�����������񣬵����쳣����ʱ������ϵͳ���쳣���������ѯ����˿ڣ�������ڵ��������ͻὫ�쳣���͸��������������������Ȼ���쳣�����Ȩ����
//
//�����в��ٷ�������������Խ����Զ˿ڹ���ķ����ƹ������Ǻǣ�������������㣬�������ֲ�������ֱ���ϻ�ȡ���õ��Զ˿ڵ���ش��롣
//
//typedef struct _DEBUGED_PROCESS_ENTRY
//{
//	LIST_ENTRY EventList;
//	PEPROCESS Process;
//	PDEBUG_OBJECT DebugPort;
//	ULONG KeExceptionDispatchCount;
//}DEBUGED_PROCESS_ENTRY, *PDEBUGED_PROCESS_ENTRY;
//
//
//
//LIST_ENTRY DebugedProcessList;
//
//PDEBUG_OBJECT GetDebugPort(PEPROCESS Process)
//{
//	PDEBUG_OBJECT DebugObject = NULL;
//	PLIST_ENTRY Entry;
//	PDEBUGED_PROCESS_ENTRY DebugedProcess;
//
//	ExAcquireFastMutex(&DebugedProcessListMutex);
//
//	for (Entry = DebugedProcessList.Flink; Entry != &DebugedProcessList; Entry = Entry->Flink)
//	{
//		DebugedProcess = CONTAINING_RECORD(Entry, DEBUGED_PROCESS_ENTRY, EventList);
//		if (DebugedProcess->Process == Process)
//		{
//			DebugedProcess->KeExceptionDispatchCount++;
//			DebugObject = DebugedProcess->DebugPort;
//		}
//	}
//
//	ExReleaseFastMutex(&DebugedProcessListMutex);
//	return DebugObject;
//}
//
//NTSTATUS SetDebugPort(PEPROCESS Process, PDEBUG_OBJECT DebugProt)
//{
//	PDEBUGED_PROCESS_ENTRY DebugedProcess;
//
//	DebugedProcess = (PDEBUGED_PROCESS_ENTRY)ExAllocatePoolWithTag(NonPagedPool,
//		sizeof(DEBUGED_PROCESS_ENTRY),
//		'DotP');
//
//	if (DebugedProcess)
//	{
//		ExAcquireFastMutex(&DebugedProcessListMutex);
//
//		DebugedProcess->Process = Process;
//		DebugedProcess->DebugPort = DebugProt;
//		DebugedProcess->KeExceptionDispatchCount = 0;
//		InsertTailList(&DebugedProcessList, &DebugedProcess->EventList);
//
//		ExReleaseFastMutex(&DebugedProcessListMutex);
//
//		return STATUS_SUCCESS;
//	}
//	else
//	{
//		return STATUS_INSUFFICIENT_RESOURCES;
//	}
//}
//
//���������ĺ��Ĵ��룬ע�⿴ע�͡������ DeviceIoControlDispatch ��������������
//
//NTSTATUS
//OnNtDebugActiveProcess(
//	IN HANDLE ProcessHandle,
//	IN HANDLE DebugObjectHandle
//	)
//{
//	_KdPrint(("NtDebugActiveProcess %x, %x\r\n", ProcessHandle, DebugObjectHandle));
//
//	NTSTATUS Status;
//	KPROCESSOR_MODE PreviousMode;
//	PEPROCESS Process;
//	PDEBUG_OBJECT DebugObject;
//	LIST_ENTRY TempList;
//
//	PAGED_CODE();
//
//	PreviousMode = KeGetPreviousMode();
//
//	Status = ObReferenceObjectByHandle(ProcessHandle,
//		PROCESS_SET_PORT,
//		*PsProcessType,
//		PreviousMode,
//		(PVOID*)&Process,
//		NULL);
//
//	if (!NT_SUCCESS(Status))
//	{
//		return Status;
//	}
//
//	DebugObject = GetDebugPort(Process);
//	if (DebugObject)
//	{
//		Status = STATUS_PORT_ALREADY_SET;
//	}
//
//	if (NT_SUCCESS(Status))
//	{
//		//
//		// ����ϵͳ�ĵ��Խ��̷��Ӻ���
//		//
//		_KdPrint(("TrueNtDebugActiveProcess Enter\r\n"));
//		Status = NtDebugActiveProcess(ProcessHandle, DebugObjectHandle);
//		_KdPrint(("TrueNtDebugActiveProcess Leave\r\n"));
//		if (NT_SUCCESS(Status))
//		{
//			Status = ObReferenceObjectByHandle(DebugObjectHandle,
//				DEBUG_PROCESS_ASSIGN,
//				*DbgkDebugObjectType,
//				PreviousMode,
//				(PVOID*)&DebugObject,
//				NULL);
//
//			if (NT_SUCCESS(Status))
//			{
//				//
//				// �޺۵��ԣ���Ҫ������Ա�־�������¼��б�
//				//
//				TempList = DebugObject->EventList;
//
//				//
//				// �ӵ��Զ������Ƴ��¼��б�
//				//
//				InitializeListHead(&DebugObject->EventList);
//
//				//
//				// ������̵ĵ��Զ˿ڼ����Ա�־��������������Զ˿��ˣ����Լ���ֱ�������ˡ�
//				//
//				DbgkClearProcessDebugObject(Process, DebugObject);
//
//				//
//				// �ָ����Զ�����¼��б�
//				//
//				DebugObject->EventList = TempList;
//
//				SetDebugPort(Process, DebugObject);
//
//				if (!NT_SUCCESS(Status))
//				{
//					ObDereferenceObject(DebugObject);
//				}
//			}
//
//		}
//	}
//
//	if (!NT_SUCCESS(Status))
//	{
//		ObDereferenceObject(Process);
//	}
//
//	return Status;
//}
//
//������ OnNtDebugActiveProcess ����������ȫ�����ԣ���Ϊ�������������������Զ˿��ˣ����Լ���ֱ�������ˡ�Ȼ���Ҫ��DbgkForwardException�¹��ӣ��������Լ�����ĵ��Զ˿ڷ������á�������롣
//
//BOOLEAN
//OnDbgkForwardException(
//	IN PEXCEPTION_RECORD ExceptionRecord,
//	IN BOOLEAN DebugException,
//	IN BOOLEAN SecondChance
//	)
//{
//	DBGKM_APIMSG m;
//	PDBGKM_EXCEPTION args;
//	NTSTATUS st;
//
//	PAGED_CODE();
//
//	if (!DebugException)
//	{
//		return FALSE;
//	}
//
//	//
//	// ���������˵��Զ˿ڣ�����û��Ӱ�䵱ǰ���̵ĵ��Զ˿�
//	//
//	if (GetDebugPort(PsGetCurrentProcess()) == NULL)
//	{
//		return FALSE;
//	}
//
//	//
//	// ���Զ˿ڱ����㡣
//	//
//	_KdPrint(("DbgkForwardException ExceptionAddress = %08x, ExceptionCode = %08x \r\n", ExceptionRecord->ExceptionAddress, ExceptionRecord->ExceptionCode));
//	args = &m.u.Exception;
//
//	//
//	// Initialize the debug LPC message with default information.
//	//
//
//	_KdPrint(("DBGKM_FORMAT_API_MSG\r\n"));
//	DBGKM_FORMAT_API_MSG(m, DbgKmExceptionApi, sizeof(*args));
//
//	//
//	// Fill in the remainder of the debug LPC message.
//	//
//
//	_KdPrint(("args->ExceptionRecord = *ExceptionRecord\r\n"));
//	args->ExceptionRecord = *ExceptionRecord;
//	args->FirstChance = !SecondChance;
//
//	//
//	// Send the debug message to the destination LPC port.
//	//
//	_KdPrint(("DbgkpSendApiMessage\r\n"));
//	st = DbgkpSendApiMessage(&m, DebugException);
//
//
//	//
//	// If the send was not successful, then return a FALSE indicating that
//	// the port did not handle the exception. Otherwise, if the debug port
//	// is specified, then look at the return status in the message.
//	//
//
//	if (!NT_SUCCESS(st) ||
//		((DebugException) &&
//			(m.ReturnedStatus == DBG_EXCEPTION_NOT_HANDLED || !NT_SUCCESS(m.ReturnedStatus))))
//	{
//		_KdPrint(("DbgkForwardException FAILED, STATUS = %08x, DebugException = %d, ReturnedStatus = %08x!\r\n", st, DebugException, m.ReturnedStatus));
//		return FALSE;
//	}
//	else
//	{
//		_KdPrint(("DbgkForwardException SUCCESSED!\r\n"));
//		return TRUE;
//	}
//}
//
//��������������õ��˼���û�е����ĺ��ĺ����������Ҫ�Լ���λ��صĺ����ˣ�����Ͳ���չ����λ�ķ����ˡ�
//
//��������Ӧ�ò���Ҫ��ӹ��Ӻ�����DbgNtDebugActiveProcess64����ǰ��� DbgNtDebugActiveProcess64 �Ĳ���ڲ��ٵ��� TrueNtDebugActiveProcess������ͨ�� DeviceIoControl ���������Լ���NtDebugActiveProcess
//
////
//// NtDebugActiveProcess
////
//NTSTATUS WINAPI DbgNtDebugActiveProcess64(
//	__in HANDLE ProcessHandle,
//	__in HANDLE DebugObjectHandle)
//{
//	NTSTATUS Status;
//
//	DWORD lResultLength;
//
//	PARAM_NtDebugActiveProcess64 param;
//	param.ProcessHandle = ProcessHandle;
//	param.DebugObjectHandle = DebugObjectHandle;
//
//	DeviceIoControl(
//		g_hModuleDbgHelper,
//		IOCTL_NtDebugActiveProcess,
//		&param,
//		sizeof(param),
//		&Status,
//		sizeof(Status),
//		&lResultLength,
//		0);
//
//
//	KdPrint((_T("NtDebugActiveProcess ����ֵ: 0x%08x!\r\n"), Status));
//
//	if (NT_SUCCESS(Status))
//	{
//		LPVOID ReturnAddress = (LPSTR)&ProcessHandle - sizeof(DWORD);
//		*(LPDWORD)ReturnAddress += 0x1E;
//	}
//	return Status;
//}
//
//3������������Ȩ����ϵͳ�ĵ���������ķ���Ȩ�ޱ�����Ϊ��ʱ������Ϊ���Ӳ������г�����Ϸ���������Ǵ����ĵ��Զ����Ϊû�е���Ȩ�޵ĵ��Զ��񡣴��������ؽ�һ�����Զ������ͣ�Ȼ���滻��ϵͳ�ĵ��Զ������͡�
//
//NTSTATUS
//DbgkCreateDebugObjectType(
//	POBJECT_TYPE* DebugObjectType
//	)
//{
//	NTSTATUS Status;
//	UNICODE_STRING Name;
//	POBJECT_TYPE SysDebugObjectType;
//	POBJECT_TYPE ObpTypeObjectType;
//
//	PAGED_CODE();
//
//	//
//	// �ȴ�ϵͳ����ĵ��Զ���
//	//
//	RtlInitUnicodeString(&Name, L"\\ObjectTypes\\DebugObject");
//	ObpTypeObjectType = ObGetObjectType(*PsProcessType);
//	Status = ObReferenceObjectByName(&Name, OBJ_CASE_INSENSITIVE, NULL, 0, ObpTypeObjectType, KernelMode, NULL, (PVOID*)&SysDebugObjectType);
//
//	if (NT_SUCCESS(Status))
//	{
//		//
//		// ϵͳ�������Ķ���رպ���
//		//
//		(OB_CLOSE_METHOD&)SysDbgkpCloseObject = SysDebugObjectType->TypeInfo.CloseProcedure;
//
//		//
//		// �����Ŵ����ǵĵ��Զ������ͣ�����򿪳ɹ���ֱ��ʹ�ü���
//		//
//		RtlInitUnicodeString(&Name, L"\\ObjectTypes\\NewDebugObject");
//		Status = ObReferenceObjectByName(&Name, OBJ_CASE_INSENSITIVE, NULL, 0, ObpTypeObjectType, KernelMode, NULL, (PVOID*)DebugObjectType);
//
//		if (Status == STATUS_OBJECT_NAME_NOT_FOUND)
//		{
//			RtlInitUnicodeString(&Name, L"NewDebugObject");
//
//			GENERIC_MAPPING GenericMapping = { STANDARD_RIGHTS_READ | DEBUG_READ_EVENT,
//				STANDARD_RIGHTS_WRITE | DEBUG_PROCESS_ASSIGN,
//				STANDARD_RIGHTS_EXECUTE | SYNCHRONIZE,
//				DEBUG_ALL_ACCESS };
//
//			OBJECT_TYPE_INITIALIZER oti = { 0 };
//			oti.Length = sizeof(oti);
//			oti.SecurityRequired = TRUE;
//			oti.InvalidAttributes = 0;
//			oti.PoolType = NonPagedPool;
//			oti.DeleteProcedure = SysDebugObjectType->TypeInfo.DeleteProcedure;
//			oti.CloseProcedure = (OB_CLOSE_METHOD)DbgkpCloseObject;
//			oti.ValidAccessMask = DEBUG_ALL_ACCESS;
//			oti.GenericMapping = GenericMapping;
//			oti.DefaultPagedPoolCharge = 0;
//			oti.DefaultNonPagedPoolCharge = 0;
//
//			Status = ObCreateObjectType(&Name, &oti, NULL, DebugObjectType);
//		}
//		else if (NT_SUCCESS(Status))
//		{
//			(*DebugObjectType)->TypeInfo.CloseProcedure = (OB_CLOSE_METHOD)DbgkpCloseObject;
//		}
//		ObDereferenceObject(SysDebugObjectType);
//	}
//	return Status;
//}
//
//NTSTATUS DbgkInitialize()
//{
//	POBJECT_TYPE NewDebugObjectType;
//	Status = DbgkCreateDebugObjectType(&NewDebugObjectType);
//
//	if (NT_SUCCESS(Status))
//	{
//		*DbgkDebugObjectType = NewDebugObjectType;
//	}
//}
//
//DbgkDebugObjectType Ϊϵͳ�ں˵����������ַ������ͨ������������ȡ��ַ��TP��������������õĸ��ӻ���������ʧ�ܣ�Ȼ����������ôһ�£���������Ͱ����ˡ�