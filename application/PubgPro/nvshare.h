#include "Game.h"

//use nv windows....
#define NVWND_PROGRAM L"NVIDIA Share.exe"
//#define HJWND_PROGRAM L"notepad.exe"
#define MAX_CLASSNAME 255
#define MAX_WNDNAME 255
struct WindowsFinderParams {
	DWORD pidOwner = NULL;
	wstring wndClassName = L"";
	wstring wndName = L"";
	RECT pos = { 0, 0, 0, 0 };
	POINT res = { 0, 0 };
	float percentAllScreens = 0.0f;
	float percentMainScreen = 0.0f;
	DWORD style = NULL;
	DWORD styleEx = NULL;
	bool satisfyAllCriteria = false;
	vector<HWND> hwnds;
};
vector<HWND> WindowsFinder(WindowsFinderParams params);
BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam);
HWND getNVWindow();
vector<DWORD> GetPIDs(wstring targetProcessName);

vector<HWND> WindowsFinder(WindowsFinderParams params) {
	EnumWindows(EnumWindowsCallback, (LPARAM)&params);
	return params.hwnds;
}

BOOL CALLBACK EnumWindowsCallback(HWND hwnd, LPARAM lParam) {
	WindowsFinderParams& params = *(WindowsFinderParams*)lParam;

	unsigned char satisfiedCriteria = 0, unSatisfiedCriteria = 0;

	// If looking for windows of a specific PDI
	DWORD pid = 0;
	GetWindowThreadProcessId(hwnd, &pid);
	if (params.pidOwner != NULL)
		if (params.pidOwner == pid)
			++satisfiedCriteria; // Doesn't belong to the process targeted
		else
			++unSatisfiedCriteria;

	// If looking for windows of a specific class
	wchar_t className[MAX_CLASSNAME] = L"";
	GetClassNameW(hwnd, className, MAX_CLASSNAME);
	wstring classNameWstr = className;
	if (params.wndClassName != L"")
		if (params.wndClassName == classNameWstr)
			++satisfiedCriteria; // Not the class targeted
		else
			++unSatisfiedCriteria;

	// If looking for windows with a specific name
	wchar_t windowName[MAX_WNDNAME] = L"";
	GetWindowTextW(hwnd, windowName, MAX_CLASSNAME);
	wstring windowNameWstr = windowName;
	if (params.wndName != L"")
		if (params.wndName == windowNameWstr)
			++satisfiedCriteria; // Not the class targeted
		else
			++unSatisfiedCriteria;

	// If looking for window at a specific position
	RECT pos;
	GetWindowRect(hwnd, &pos);
	if (params.pos.left || params.pos.top || params.pos.right || params.pos.bottom)
		if (params.pos.left == pos.left && params.pos.top == pos.top && params.pos.right == pos.right && params.pos.bottom == pos.bottom)
			++satisfiedCriteria;
		else
			++unSatisfiedCriteria;

	// If looking for window of a specific size
	POINT res = { pos.right - pos.left, pos.bottom - pos.top };
	if (params.res.x || params.res.y)
		if (res.x == params.res.x && res.y == params.res.y)
			++satisfiedCriteria;
		else
			++unSatisfiedCriteria;

	// If looking for windows taking more than a specific percentage of all the screens
	float ratioAllScreensX = res.x / GetSystemMetrics(SM_CXSCREEN);
	float ratioAllScreensY = res.y / GetSystemMetrics(SM_CYSCREEN);
	float percentAllScreens = ratioAllScreensX * ratioAllScreensY * 100;
	if (params.percentAllScreens != 0.0f)
		if (percentAllScreens >= params.percentAllScreens)
			++satisfiedCriteria;
		else
			++unSatisfiedCriteria;

	// If looking for windows taking more than a specific percentage or the main screen
	RECT desktopRect;
	GetWindowRect(GetDesktopWindow(), &desktopRect);
	POINT desktopRes = { desktopRect.right - desktopRect.left, desktopRect.bottom - desktopRect.top };
	float ratioMainScreenX = res.x / desktopRes.x;
	float ratioMainScreenY = res.y / desktopRes.y;
	float percentMainScreen = ratioMainScreenX * ratioMainScreenY * 100;
	if (params.percentMainScreen != 0.0f)
		if (percentAllScreens >= params.percentMainScreen)
			++satisfiedCriteria;
		else
			++unSatisfiedCriteria;

	// Looking for windows with specific styles
	LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
	if (params.style)
		if (params.style & style)
			++satisfiedCriteria;
		else
			++unSatisfiedCriteria;

	// Looking for windows with specific extended styles
	LONG_PTR styleEx = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
	if (params.styleEx)
		if (params.styleEx & styleEx)
			++satisfiedCriteria;
		else
			++unSatisfiedCriteria;

	if (!satisfiedCriteria)
		return TRUE;

	if (params.satisfyAllCriteria && unSatisfiedCriteria)
		return TRUE;

	// If looking for multiple windows
	params.hwnds.push_back(hwnd);
	return TRUE;
}

void closeNVWinndow()
{
	vector<DWORD> existingNotepads = GetPIDs(NVWND_PROGRAM);
	if (!existingNotepads.empty()) {
		for (int i(0); i < existingNotepads.size(); ++i) {
			// Terminating processes
			HANDLE hOldProcess = OpenProcess(PROCESS_TERMINATE, FALSE, existingNotepads[i]);
			TerminateProcess(hOldProcess, 0);
			CloseHandle(hOldProcess);
		}
	}
}

HWND getNVWindow() {
	HWND hwndHiHjacked = NULL;

	// Remove previous windows
	//vector<DWORD> existingNotepads = GetPIDs(HJWND_PROGRAM);
	//if (!existingNotepads.empty()) {
	//	for (int i(0); i < existingNotepads.size(); ++i) {
	//		// Terminating processes
	//		HANDLE hOldProcess = OpenProcess(PROCESS_TERMINATE, FALSE, existingNotepads[i]);
	//		TerminateProcess(hOldProcess, 0);
	//		CloseHandle(hOldProcess);
	//	}
	//}
	closeNVWinndow();

	//std::string startCmd = "start C:\\\"Program Files\"\\\"NVIDIA Corporation\"\\\"NVIDIA GeForce Experience\"\\\"NVIDIA Share.exe\"";
	//return NULL;

	system("start C:\\\"Program Files\"\\\"NVIDIA Corporation\"\\\"NVIDIA GeForce Experience\"\\\"NVIDIA Share.exe\""); // Start notepad, and not as child process, so easy :)

	Sleep(5000);


	/*
	// Starts notepad (as child process, unsafe)
	wchar_t wWinDir[MAX_PATH] = L"";
	GetWindowsDirectory(wWinDir, MAX_PATH);
	wstring winDir = wWinDir;
	wstring notepadExe = winDir + L"\\notepad.exe";
	STARTUPINFO si;
	SecureZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi;
	SecureZeroMemory(&pi, sizeof(pi));
	if (!CreateProcess(notepadExe.c_str(), 0, 0, 0, 0, 0, 0, 0, &si, &pi))
	return false;
	*/

	// Finding notepad's window (we could just use FindWindow but then it would be OS language dependent)
	vector<DWORD> notepads = GetPIDs(NVWND_PROGRAM);
	//if (notepads.empty() || notepads.size() > 1) // Should check if more than one to be more strict
	//	return hwndHiHjacked;
	WindowsFinderParams params;
	params.pidOwner = notepads[0];
	params.style = WS_VISIBLE;
	params.satisfyAllCriteria = true;
	vector<HWND> hwnds;
	int attempt = 0; // The process takes a bit of time to initialise and spawn the window, will try during 5 sec before time out
	while (hwndHiHjacked == NULL || attempt > 50) {
		Sleep(100);
		hwnds = WindowsFinder(params);
		if (hwnds.size() > 1)
			return hwndHiHjacked;
		hwndHiHjacked = hwnds[0];
		++attempt;
	}
	if (!hwndHiHjacked)
		return hwndHiHjacked;

	// Making the window usable for overlay puposes
	SetMenu(hwndHiHjacked, NULL);
	SetWindowLongPtr(hwndHiHjacked, GWL_STYLE, WS_VISIBLE);
	SetWindowLongPtr(hwndHiHjacked, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT); // WS_EX_NOACTIVATE  and WS_EX_TOOLWINDOW removes it from taskbar
	//SetWindowPos(hwndHiHjacked, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW); // DX fails to init if I do that here for some reason

	// Quick and dirty transparency
	SetLayeredWindowAttributes(hwndHiHjacked, RGB(0, 0, 0), 0, LWA_COLORKEY);

	return hwndHiHjacked;
}

vector<DWORD> GetPIDs(wstring targetProcessName) {
	vector<DWORD> pids;
	if (targetProcessName == L"")
		return pids;
	HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32W entry;
	entry.dwSize = sizeof entry;
	if (!Process32FirstW(snap, &entry)) {
		CloseHandle(snap);
		return pids;
	}
	do {
		if (wstring(entry.szExeFile) == targetProcessName) {
			pids.emplace_back(entry.th32ProcessID);
		}
	} while (Process32NextW(snap, &entry));
	CloseHandle(snap);
	return pids;
}

//end of use nv windows