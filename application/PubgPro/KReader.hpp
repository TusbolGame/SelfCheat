#ifndef HEAD_KREADER_H
#define HEAD_KREADER_H
#include <string>
#include <iostream>
#include <Windows.h>
#include <winioctl.h>
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
//#define IS_HEX(x) (((x) >= '0'  && (x) <= '9') || ((x) >= 'A' && (x) <= 'F') || ((x) >= 'a' && (x) <= 'f'))
//#define HEX_V(x) ((x) <= '9' ? ((x) - '0') : ((x) <= 'F' ? ((x) - 'A' + 0xa) : ((x) - 'a' + 0xa)))
//#define HEX_S(x) (x >= 0xa ? (x - 0xa + 'A') : (x + '0'))
//#define BUF_SIZE (128 * 1024)
using namespace std;



typedef struct WPMS {
	ULONG64 address;
	ULONG32 size;
}*pWPMStruct,WPMStruct;







class CMem
{
public:
	CMem() : m_hDevice(nullptr), lpBase(nullptr), nOutput(0)
	{

	}
	CMem(DWORD pid) : m_hDevice(nullptr), lpBase(nullptr), nOutput(0),Pid(pid)
	{
		m_hDevice = CreateFile(L"\\\\.\\lf9WAQOjp8", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (m_hDevice != INVALID_HANDLE_VALUE) {
			DeviceIoControl(m_hDevice, IOCTL_InputProcessId, (LPVOID)&pid, 0x4, NULL, 0, &nOutput, NULL);
			DeviceIoControl(m_hDevice, IOCTL_OutputlpBaseAddress, NULL, 0, &lpBase, 0x8, &nOutput, NULL);
		}
	}

	DWORD_PTR GetBase()
	{
		return (DWORD_PTR)lpBase;
	}
	DWORD GetPid() {
		return Pid;
	}
	BOOL WPM(SIZE_T address, LPVOID TowriteAddress, DWORD bufSize) {
		DeviceIoControl(m_hDevice, IOCTL_InputBaseAddress, (LPVOID)&address, 0x8, NULL, 0, &nOutput, NULL);
		DeviceIoControl(m_hDevice, IOCTL_InputReadOrWriteLen, (LPVOID)&bufSize, 0x4, NULL, 0, &nOutput, NULL);
		return DeviceIoControl(m_hDevice, IOCTL_KWriteProcessMemory, TowriteAddress, bufSize, NULL, 0, &nOutput, NULL);
	}


	template<typename T> 
	T RPM(const SIZE_T& address)
	{
		T buff;
		WPMStruct readStruct;
		readStruct.address = address;
		readStruct.size = sizeof(T);
		DeviceIoControl(m_hDevice, IOCTL_Read, (LPVOID)&readStruct, sizeof(WPMStruct), (LPVOID)&buff, readStruct.size, &nOutput, NULL);
		return buff;
	}

	template<typename T> T RPM(const SIZE_T& address, const uint32_t& bufSize)
	{
		T buff;
		WPMStruct readStruct;
		readStruct.address = address;
		readStruct.size = bufSize;
		DeviceIoControl(m_hDevice, IOCTL_Read, (LPVOID)&readStruct, sizeof(WPMStruct), (LPVOID)&buff, bufSize, &nOutput, NULL);
		return buff;
	}

	char * readSize(const int64_t& address, const int32_t& bufSize) {

		char* writeMe = new char[bufSize];
		WPMStruct readStruct;
		readStruct.address = address;
		readStruct.size = bufSize;
		DeviceIoControl(m_hDevice, IOCTL_Read, (LPVOID)&readStruct, sizeof(WPMStruct), writeMe, bufSize, &nOutput, NULL);
		return writeMe;
	}
	BOOL ReadS(const unsigned __int64& address,PVOID buff,const uint32_t& bufSize) {
		WPMStruct readStruct;
		readStruct.address = address;
		readStruct.size = bufSize;
		return DeviceIoControl(m_hDevice, IOCTL_Read, (LPVOID)&readStruct, sizeof(WPMStruct), (LPVOID)buff, bufSize, &nOutput, NULL);
	}
	BOOL  RPMWSTR(const int64_t& address, const int32_t& bufSize, wchar_t * readAddress) {
		WPMStruct readStruct;
		readStruct.address = address;
		readStruct.size = bufSize;
		return DeviceIoControl(m_hDevice, IOCTL_Read, (LPVOID)&readStruct, sizeof(WPMStruct), (LPVOID)readAddress, bufSize, &nOutput, NULL);
	}

	void MCreateThread(const int64_t& ShellcodeAddress) {
		DeviceIoControl(m_hDevice, IOCTL_CreateThread, (LPVOID)&ShellcodeAddress, 0x8, NULL, 0, &nOutput, NULL);
	}
	void Close() {
		CloseHandle(m_hDevice);
	}
private:
	ULONG nOutput;//返回字节大小
	HANDLE m_hDevice;
	LPVOID lpBase;
	DWORD Pid;
};
#endif