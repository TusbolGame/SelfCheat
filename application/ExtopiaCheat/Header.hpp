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
//	Physical_address.u.LowPart = ProcessCr3Low; //指向该进程的页目录表地址
//	Physical_address.u.HighPart = ProcessCr3Hig; //指向该进程的超空间(Hyper Space)的页目录表地址
//	DbgBreakPoint();
//
//	//cr3 = Page Map Level 4
//	//win10 1803以上系统不能直接拿到cr3页面的物理内存地址了，通过mdl映射
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
//	BOOL    IsError = 0;//定义一个错误标识
//	DWORD    LastError = 0;//定义一个错误代码
//	MEMORY_BASIC_INFORMATION    lpBuffer = { 0 };//定义一个VirtualQueryEx的参数的结构
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
//						if (MessageBox(NULL, TEXT("权限不足！请提升权限！\r\n是否提升权限？"), TEXT("权限不足！"), 48 + MB_YESNO))
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
//	MessageBox(NULL, TEXT("找不到任务管理器，请运行任务管理器!"), TEXT("错误"), 48);
//	CloseHandle(hSnapShot);
//	return    0;
//Next:
//	RegionSize = sizeof(PVOID);
//	if (::MmAllocateVirtualMemoryEx(hProcess, &Address, 0, &RegionSize, MEM_COMMIT, PAGE_READWRITE))//向任务管理器进程申请内存
//	{
//		if (::MmVirtualQueryEx(hProcess, Address, &lpBuffer, RegionSize) != 0)//查询申请的内存属性
//		{
//			MessageBox(NULL, TEXT("程序运行成功!"), TEXT("错误"), 0);
//		}
//		::MmFreeVirtualMemoryEx(hProcess, &Address, &RegionSize, MEM_RELEASE);//释放申请的内存属性
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
//		LPCTSTR    ErrorStr = ::GetLastErrorMessageW(LastError);//返回错误代码的格式化字符
//		MessageBox(NULL, ErrorStr, TEXT("错误"), 16);
//		::FreeMemory(ErrorStr);//释放内存，只能释放由我的程序重写的代码申请的内存，不能用作释放别的代码申请的内存
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
//	if (!::ProbeForWrite(lpBuffer, sizeof(MEMORY_BASIC_INFORMATION)))//进行参数检查，增加程序稳定性，并非必要，可以去除
//		return  FALSE;
//	Status = ZwQueryVirtualMemory(hProcess, (PVOID)lpAddress, MemoryBasicInformation, lpBuffer, sizeof(MEMORY_BASIC_INFORMATION), &ReturnSize);//构建参数，调用ZwQueryVirtualMemory
//	if (!NT_SUCCESS(Status))//返回一个NT错误，用宏NT_SUCCESS判断错误类型，如果NT_SUCCESS判断成功，即返回true则表明系统执行正常，否则系统执行出错
//	{
//		//系统执行出错
//		::BaseSetLastNTError(Status);//自定义函数，功能将NT错误，转换为系统错误，并且调用SetLastError来设置错误
//		return  FALSE;//返回一个FALSE，也就是0；
//	}
//	return  ReturnSize;//调用成功，返回内存块大小，这里还应该先调用一个::BaseSetLastNTError(NOERROR);的，用来表示系统调用没有错误，我给忘了！
//					   /*return  VirtualQueryEx(
//					   hProcess,
//					   lpAddress,
//					   lpBuffer,
//					   dwLength);*/
//}
//
//
//1、R3应用层下，DebugActiveProcess 加载调试器时，会设置一个远程断点，而这个远程断点其实多此一举，反而有时会触发反调试检测。唯一要做的就是绕过它。
//
//这是NtDebugActiveProcess的上层函数，DbgUiDebugActiveProcess的反汇编代码：
//_DbgUiDebugActiveProcess@4:
//00:  mov         edi, edi
//7760FC4C  push        ebp
//7760FC4D  mov         ebp, esp
//7760FC4F  mov         eax, dword ptr fs : [00000018h]
//7760FC55  push        esi
//7760FC56  push        dword ptr[eax + 0F24h]
//7760FC5C  push        dword ptr[ebp + 8]
//7760FC5F  call        _NtDebugActiveProcess@8 (775909A4h)
//7760FC64  mov         esi, eax        // 原始返回地址
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
//7760FC82  pop         esi        // +0x1E后的返回地址
//7760FC83  pop         ebp
//7760FC84  ret         4
//
//下面是64位 WINDOW7 下面的 NtDebugActiveProcess 的钩子函数的具体代码，绕过远程断点的方法是在返回地址上加偏移0x1E
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
//	KdPrint((_T("NtDebugActiveProcess 返回值: 0x%08x!\r\n"), Status));
//
//	//
//	// 绕过应用层的远程断点，完全多此一举啊。
//	//
//	if (NT_SUCCESS(Status))
//	{
//		LPVOID ReturnAddress = (LPSTR)&ProcessHandle - sizeof(DWORD);
//		*(LPDWORD)ReturnAddress += 0x1E;
//	}
//	return Status;
//}
//
//2、如果保护程序有驱动，那么在R0核心层下调试端口清零会是绝大多数游戏保护的选择。进程的调试端口保存了一个调试器对像，当有异常发生时，操作系统的异常处理程序会查询这个端口，如果存在调试器，就会将异常发送给调试器，调试器会优先获得异常处理的权利。
//
//网上有不少方法，这儿采用自建调试端口管理的方法绕过它，呵呵，你想清零就清零，反正我又不用它。直接上获取设置调试端口的相关代码。
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
//这是真正的核心代码，注意看注释。后面的 DeviceIoControlDispatch 会调用这个函数。
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
//		// 调用系统的调试进程符加函数
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
//				// 无痕调试，需要清理调试标志，保存事件列表
//				//
//				TempList = DebugObject->EventList;
//
//				//
//				// 从调试对像中移除事件列表
//				//
//				InitializeListHead(&DebugObject->EventList);
//
//				//
//				// 清理进程的调试端口及调试标志，不用你清零调试端口了，我自己就直接清零了。
//				//
//				DbgkClearProcessDebugObject(Process, DebugObject);
//
//				//
//				// 恢复调试对像的事件列表
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
//但是用 OnNtDebugActiveProcess 加载那是完全不调试，因为这个函数不用你清零调试端口了，我自己就直接清零了。然后就要对DbgkForwardException下钩子，让我们自己保存的调试端口发挥作用。具体代码。
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
//	// 进程设置了调试端口，或者没有影射当前进程的调试端口
//	//
//	if (GetDebugPort(PsGetCurrentProcess()) == NULL)
//	{
//		return FALSE;
//	}
//
//	//
//	// 调试端口被置零。
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
//上面的两个函数用到了几个没有导出的核心函数，这就需要自己定位相关的函数了，这里就不再展开定位的方法了。
//
//调试器在应用层需要添加钩子函数：DbgNtDebugActiveProcess64，与前面的 DbgNtDebugActiveProcess64 的差别在不再调用 TrueNtDebugActiveProcess，而是通过 DeviceIoControl 调用我们自己的NtDebugActiveProcess
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
//	KdPrint((_T("NtDebugActiveProcess 返回值: 0x%08x!\r\n"), Status));
//
//	if (NT_SUCCESS(Status))
//	{
//		LPVOID ReturnAddress = (LPSTR)&ProcessHandle - sizeof(DWORD);
//		*(LPDWORD)ReturnAddress += 0x1E;
//	}
//	return Status;
//}
//
//3、调试器对像降权。当系统的调试器对像的访问权限被设置为零时，表现为附加不上所有程序，游戏保护让我们创建的调试对像成为没有调试权限的调试对像。处理方法：重建一个调试对像类型，然后替换掉系统的调试对像类型。
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
//	// 先打开系统本身的调试对像
//	//
//	RtlInitUnicodeString(&Name, L"\\ObjectTypes\\DebugObject");
//	ObpTypeObjectType = ObGetObjectType(*PsProcessType);
//	Status = ObReferenceObjectByName(&Name, OBJ_CASE_INSENSITIVE, NULL, 0, ObpTypeObjectType, KernelMode, NULL, (PVOID*)&SysDebugObjectType);
//
//	if (NT_SUCCESS(Status))
//	{
//		//
//		// 系统调试器的对像关闭函数
//		//
//		(OB_CLOSE_METHOD&)SysDbgkpCloseObject = SysDebugObjectType->TypeInfo.CloseProcedure;
//
//		//
//		// 先试着打开我们的调试对像类型，如果打开成功，直接使用即可
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
//DbgkDebugObjectType 为系统内核调试器对像地址，可以通过符号搜索获取地址。TP在启动后会让所用的附加或启动调试失败，然后我们来这么一下，整个世界就安静了。