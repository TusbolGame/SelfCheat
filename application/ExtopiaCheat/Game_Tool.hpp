#pragma once


#ifndef GAME_TOOL_H
#define GAME_TOOL_H

#include <windows.h>
#include <tlhelp32.h>
#include <iostream>


std::hash<std::wstring> WHashfun;

static int32_t GetGameProcessID()
{
	HANDLE snapshot;
	PROCESSENTRY32 processinfo;
	processinfo.dwSize = sizeof(processinfo);
	snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (snapshot == NULL)
		return 0;
	BOOL status = Process32First(snapshot, &processinfo);
	while (status)
	{
		if (13508079142702545337 == WHashfun(processinfo.szExeFile))
			return processinfo.th32ProcessID;
		status = Process32Next(snapshot, &processinfo);
	}
	return 0;
}

static uint32_t GameIshaving() {
	uint32_t pid = 0;
	while (!(pid = GetGameProcessID()))
		Sleep(1000);
	return pid;
}


#endif