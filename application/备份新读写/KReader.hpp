#ifndef HEAD_KREADER_H
#define HEAD_KREADER_H
#include <string>
#include <iostream>
#include <Windows.h>
#include <winioctl.h>
#define IOCTL_Read						   CTL_CODE(FILE_DEVICE_UNKNOWN,0x905,METHOD_BUFFERED,FILE_ANY_ACCESS) 
#define IOCTL_Write						   CTL_CODE(FILE_DEVICE_UNKNOWN,0x906,METHOD_BUFFERED,FILE_ANY_ACCESS) 
#define IOCTL_InputRWPid                   CTL_CODE(FILE_DEVICE_UNKNOWN,0x907,METHOD_BUFFERED,FILE_ANY_ACCESS) 
#define IOCTL_GetRWBase					   CTL_CODE(FILE_DEVICE_UNKNOWN,0x908,METHOD_BUFFERED,FILE_ANY_ACCESS) 
//#define IS_HEX(x) (((x) >= '0'  && (x) <= '9') || ((x) >= 'A' && (x) <= 'F') || ((x) >= 'a' && (x) <= 'f'))
//#define HEX_V(x) ((x) <= '9' ? ((x) - '0') : ((x) <= 'F' ? ((x) - 'A' + 0xa) : ((x) - 'a' + 0xa)))
//#define HEX_S(x) (x >= 0xa ? (x - 0xa + 'A') : (x + '0'))
//#define BUF_SIZE (128 * 1024)
using namespace std;



typedef struct _WStruct {
	SIZE_T address;
	UINT32 size;
}WStruct, *PWStruct;







class CMem
{
public:
	CMem() : m_hDevice(nullptr), lpBase(0), nOutput(0)
	{

	}
	CMem(DWORD pid) : m_hDevice(nullptr), lpBase(0), nOutput(0),Pid(pid), Blankaddress(0)
	{
		m_hDevice = CreateFile(L"\\\\.\\lf9WAQOjp8", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (m_hDevice != INVALID_HANDLE_VALUE) {
			DeviceIoControl(m_hDevice, IOCTL_InputRWPid, &pid, 0x4, NULL, NULL, &nOutput, NULL);
			DeviceIoControl(m_hDevice, IOCTL_GetRWBase, NULL, 0, &lpBase, 0x8, &nOutput, NULL);
		}
	}

	DWORD_PTR GetBase()
	{
		return (DWORD_PTR)lpBase;
	}
	DWORD GetPid() {
		return Pid;
	}
	void WPM(SIZE_T address, LPVOID TowriteAddress, DWORD bufSize) {
		WStruct W_buffer;
		W_buffer.address = address;
		W_buffer.size = bufSize;
		UCHAR* buffer = new UCHAR[sizeof(WStruct) + W_buffer.size];
		memcpy(buffer, &W_buffer, sizeof(WStruct));
		memcpy(buffer + sizeof(WStruct), TowriteAddress, W_buffer.size);
		DeviceIoControl(m_hDevice, IOCTL_Write, buffer, sizeof(WStruct) + W_buffer.size, NULL, NULL, &nOutput, NULL);
	}


	template<typename T> 
	T RPM(const SIZE_T& address)
	{
		T buff;
		WStruct readStruct;
		readStruct.address = address;
		readStruct.size = sizeof(T);
		DeviceIoControl(m_hDevice, IOCTL_Read, (LPVOID)&readStruct, sizeof(WStruct), (LPVOID)&buff, readStruct.size, &nOutput, NULL);
		return buff;
	}

	template<typename T> T RPM(const SIZE_T& address, const uint32_t& bufSize)
	{
		T buff;
		WStruct readStruct;
		readStruct.address = address;
		readStruct.size = bufSize;
		DeviceIoControl(m_hDevice, IOCTL_Read, (LPVOID)&readStruct, sizeof(WStruct), (LPVOID)&buff, bufSize, &nOutput, NULL);
		return buff;
	}

	char * readSize(const int64_t& address, const int32_t& bufSize) {

		char* writeMe = new char[bufSize];
		WStruct readStruct;
		readStruct.address = address;
		readStruct.size = bufSize;
		DeviceIoControl(m_hDevice, IOCTL_Read, (LPVOID)&readStruct, sizeof(WStruct), writeMe, bufSize, &nOutput, NULL);
		return writeMe;
	}
	bool ReadS(const unsigned __int64& address,PVOID buff,const size_t& bufSize) {
		WStruct readStruct;
		readStruct.address = address;
		readStruct.size = bufSize;
		return DeviceIoControl(m_hDevice, IOCTL_Read, (LPVOID)&readStruct, sizeof(WStruct), (LPVOID)buff, bufSize, &nOutput, NULL);
	}
	BOOL  RPMWSTR(const int64_t& address, const int32_t& bufSize, wchar_t * readAddress) {
		WStruct readStruct;
		readStruct.address = address;
		readStruct.size = bufSize;
		return DeviceIoControl(m_hDevice, IOCTL_Read, (LPVOID)&readStruct, sizeof(WStruct), (LPVOID)readAddress, bufSize, &nOutput, NULL);
	}

	void MCreateThread(const int64_t& ShellcodeAddress) {
		//DeviceIoControl(m_hDevice, IOCTL_CreateThread, (LPVOID)&ShellcodeAddress, 0x8, NULL, 0, &nOutput, NULL);
	}
	void Close() {
		CloseHandle(m_hDevice);
	}
private:
	ULONG nOutput;//返回字节大小
	HANDLE m_hDevice;
	SIZE_T lpBase;
	DWORD Pid;
	SIZE_T Blankaddress;
};
#endif