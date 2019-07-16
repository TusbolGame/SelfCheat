#include "KReader.hpp"
#include "Game_Tool.hpp"
#include "Game_data.hpp"


#include <fstream>
#include <random>
#include <atlbase.h>


#include <stack>

CMem* mem = nullptr;
std::ofstream examplefile("name_1.txt");
RECT RRc;





void ModifyDamage() {

	//jmp add
	//mov [r12 + 0x548], 0
	//mov rcx, [r12 + 38]
	//jmp ret
	uint64_t hookAdd = mem->GetBase() + 0xA2E414;
	uint64_t blanck = mem->GetBase() + 0x470;
	uint8_t shellcode[] = {
		0x41,0xC7,0x84,0x24,0x48,0x05,0x00,0x00,0x00,0x00,0x00,0x00,
		0x49,0x8B,0x4C,0x24,0x38,
		0xE9,0x00,0x00,0x00,0x00
	};
	*(uint32_t*)(shellcode + 18) = (hookAdd + 5 - (blanck + 17) - 5);
	mem->WPM(blanck, shellcode, sizeof(shellcode));
	uint8_t jmp_code[] = {0xE9,0x00,0x00,0x00,0x00};
	*(uint32_t*)(jmp_code + 1) = (uint32_t)(blanck - hookAdd - 5);
	mem->WPM(hookAdd, jmp_code, 0x5);
}




void MainThread() {
			InitData();
			int32_t count = mem->RPM<int32_t>(Base_data::ulevel_ptr + offset::ActorArray_offset + 0x8);
			uint64_t enlist = mem->RPM<uint64_t>(Base_data::ulevel_ptr + offset::ActorArray_offset);
			uint64_t pawn = mem->RPM<uint64_t>(Base_data::controller_ptr + Actor_offset::AcknowledgedPawn_offset);
			uint64_t mesh = mem->RPM<uint64_t>(pawn + Actor_offset::USkeletalMeshComponent_offset);
			
			FCameraCacheEntry camera;
			camera.Location = mem->RPM<Vector3>(Base_data::camera_ptr+Esp_offset::camera_location);
			camera.Rotaion = mem->RPM<Vector3>(Base_data::camera_ptr + Esp_offset::camera_rotation);
			camera.Fov = mem->RPM<float>(Base_data::camera_ptr + Esp_offset::camera_fov);
			Vector3 ret = GetBoneWithRotation(mesh, 15);
			Vector3 Wpos = WorldToScreen(ret, camera);
			DrawString(Wpos.x, Wpos.y, D3DCOLOR_ARGB(255,0,255,0), pFont, "我是本人");

			//if (GetAsyncKeyState(VK_F5) & 1) {
				//int count_num = 0;
				for (int i = 0; i < count; ++i) {
					auto entity = mem->RPM<uint64_t>(enlist + i * 8);
					if (!entity)
						continue;
					auto id = mem->RPM<int32_t>(entity + 0x18);
					std::string name = GetnameFromId(id);
					//examplefile << name << std::endl;

					//if (name == "ShooterPlayerState_BR" || name == "ShooterPropSystem")
					//	continue;
					//name == "KimPawn_BR_C" || name == "SuzyPawn_BR_C" ||  这是在岛上时的name
					if (name == "KimPawn_BR_C" || name == "SuzyPawn_BR_C") {
						//count_num++;
						if (entity == pawn)
							continue;
						if (GetAsyncKeyState(VK_F5) & 1) {
							std::cout << name << std::endl;
							//std::cout << std::hex<<RootComponent + offset::RelativeLocation_offset << std::endl;
							uint64_t RootComponent = mem->RPM<uint64_t>(pawn + offset::RootComponent_offset);


							SIZE_T add = RootComponent + Esp_offset::componentToWorld_offset + 0x10;



							//Vector3 writeValue = mem->RPM<Vector3>(mem->RPM<uint64_t>(entity+offset::RootComponent_offset)+ offset::RelativeLocation_offset);


							//mem->WPM(add, &writeValue, 0xC);
						}
					}
				}
			//}
			Sleep(10);
}



void render()
{

	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	d3ddev->BeginScene();
	MainThread();
	d3ddev->EndScene();
	d3ddev->Present(NULL, NULL, NULL, NULL);
}

//set up overlay window
void SetupWindow()
{
	RECT rc;
	twnd = FindWindow(L"UnrealWindow", 0);
	if (twnd != NULL)
	{
		GetWindowRect(twnd, &rc);
		s_width = rc.right - rc.left;
		s_height = rc.bottom - rc.top;
	}
	else
	{
		Sleep(3000);
		ExitProcess(0);
	}
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(0);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(0x1000, 0x4000);
	int randNumber = dis(gen);
	wc.hbrBackground = (HBRUSH)RGB(0, 0, 0);
	char szbuff[200] = { 0 };
	_itoa_s(randNumber, szbuff, 10);
	wc.lpszClassName = CA2W(szbuff);
	RegisterClassEx(&wc);

	hWnd = CreateWindowExW(NULL, wc.lpszClassName, wc.lpszClassName, WS_POPUP | WS_VISIBLE, rc.left, rc.top, s_width, s_height, NULL, NULL, wc.hInstance, NULL);
	SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST | WS_POPUP | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);

	SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 0, ULW_COLORKEY);
	SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);

	ShowWindow(hWnd, SW_SHOW);
	initD3D(hWnd);
}

WPARAM MainLoop()
{
	MSG msg;
	RECT rc;
	while (TRUE)
	{
		ZeroMemory(&msg, sizeof(MSG));
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			exit(0);
		if (!twnd)
		{
			ExitProcess(0);
		}
		ZeroMemory(&rc, sizeof(RECT));
		GetWindowRect(twnd, &RRc);
		s_width = RRc.right - RRc.left;
		s_height = RRc.bottom - RRc.top;
		MoveWindow(hWnd, RRc.left, RRc.top, s_width, s_height, true);
		SetWindowPos(hWnd, GetWindow(twnd, GW_HWNDPREV), 0, 0, 0, 0, SWP_NOMOVE || SWP_NOSIZE);
		UpdateWindow(hWnd);
		render();
	}
	return msg.wParam;
}

//F2 0F 11 81 78 04 00 00   Rotation
//000007FEEB35F108 48 8B 05 99 F8 07 02 mov         rax, qword ptr[GWorld(07FEED3DE9A8h)]
//000007FEEB35F10F 48 3B C3             cmp         rax, rbx
//000007FEEB35F112 48 0F 44 C6          cmove       rax, rsi
//000007FEEB35F116 48 89 05 8B F8 07 02 mov         qword ptr[GWorld(07FEED3DE9A8h)], rax

int main() {
	uint32_t Game_id = GameIshaving();
	std::cout << Game_id << std::endl;
	mem = new CMem(Game_id);

	SetupWindow();
	MainLoop();
	return 0;
}


