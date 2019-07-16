#pragma once
#ifndef OVERLAY_HEAD
#define OVERLAY_HEAD

#include <d3d9.h>
#include <d3dx9.h>
#include <Dwmapi.h>

int s_width = 1920;
int s_height = 1080;


LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;
HWND hWnd;
HWND twnd;
MARGINS  margin = { 0,0,s_width,s_height };
LPD3DXFONT pFont;
LPD3DXFONT pFont2;
ID3DXLine* d3dLine;
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void DrawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char *fmt, ...);
void DrawStringX(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char *fmt, ...);
void DrawLine(float x, float y, float xx, float yy, D3DCOLOR color);
void DrawBox(float x, float y, float width, float height, D3DCOLOR color);
void FillRBG(int x, int y, int w, int h, D3DCOLOR color);
void initD3D(HWND hWnd);
void render();
void DrawBorderString(INT x, INT y, DWORD color, DWORD bordercolor, int borderw, LPD3DXFONT g_pFont, CONST CHAR*fmt, ...);


void initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);    // create the Direct3D interface

	D3DPRESENT_PARAMETERS d3dpp;    // create a struct to hold various device information

	ZeroMemory(&d3dpp, sizeof(d3dpp));    // clear out the struct for use
	d3dpp.Windowed = TRUE;    // program windowed, not fullscreen
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
	d3dpp.hDeviceWindow = hWnd;    // set the window to be used by Direct3D
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;     // set the back buffer format to 32-bit
	d3dpp.BackBufferWidth = s_width;    // set the width of the buffer
	d3dpp.BackBufferHeight = s_height;    // set the height of the buffer

	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	// create a device class using this information and the info from the d3dpp struct
	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);
	D3DXCreateFontA(d3ddev, 16, 0, FW_HEAVY, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &pFont2);
	D3DXCreateFontA(d3ddev,
		17,
		0,
		FW_MEDIUM,
		false,
		0,		
		DEFAULT_CHARSET,	
		OUT_DEFAULT_PRECIS,	//指定了窗口应该尝试匹配所需的字体大小和字体的特点与实际。使用out_tt_only_precis例如，确保你总是TrueType字体。
		ANTIALIASED_QUALITY,	//指定Windows如何应所需的字体和一个真正的字体匹配。它适用于光栅字体，不应该影响TrueType字体。
		DEFAULT_PITCH | FF_DONTCARE,	//Pitch and family index。
		"Microsoft YaHei UI Light",		//Microsoft YaHei UI Light包含字体名称的字符串。如果编译器设置需要Unicode数据类型，解决lpcwstr LPCTSTR。否则，字符串数据类型解析字符
		&pFont);
	D3DXCreateLine(d3ddev, &d3dLine);
}

void DrawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char *fmt, ...)
{
	char buf[1024] = { 0 };
	va_list va_alist;
	RECT FontPos = { x, y, x + 120, y + 16 };
	//RECT FontPos = { x, y, x + 150, y + 30 };
	va_start(va_alist, fmt);
	vsprintf_s(buf, fmt, va_alist);
	va_end(va_alist);
	g_pFont->DrawTextA(NULL, buf, -1, &FontPos, DT_NOCLIP, color);
}

void DrawBorderString(INT x, INT y, DWORD color, DWORD bordercolor, int borderw, LPD3DXFONT g_pFont, CONST CHAR*fmt, ...) {
	//描边
	RECT FontPos1 = { x - borderw,y,x + 120,y + 16 };
	RECT FontPos2 = { x ,y - borderw,x + 120,y + 16 };
	RECT FontPos3 = { x + borderw ,y,x + 120,y + 16 };
	RECT FontPos4 = { x  ,y + borderw,x + 120,y + 16 };

	RECT FontPos = { x,y,x + 120,y + 16 };
	CHAR buf[1024] = { '\0' };
	va_list va_alist;

	va_start(va_alist, fmt);
	vsprintf_s(buf, fmt, va_alist);
	va_end(va_alist);
	//Border
	g_pFont->DrawTextA(NULL, buf, -1, &FontPos1, DT_NOCLIP | DT_CENTER, bordercolor);
	g_pFont->DrawTextA(NULL, buf, -1, &FontPos2, DT_NOCLIP | DT_CENTER, bordercolor);
	g_pFont->DrawTextA(NULL, buf, -1, &FontPos3, DT_NOCLIP | DT_CENTER, bordercolor);
	g_pFont->DrawTextA(NULL, buf, -1, &FontPos4, DT_NOCLIP | DT_CENTER, bordercolor);

	//text
	g_pFont->DrawTextA(NULL, buf, -1, &FontPos, DT_NOCLIP | DT_CENTER, bordercolor);
}

void DrawStringX(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char *fmt, ...)
{
	//FillRBG(x-1, y-1, 50, 17, D3DCOLOR_ARGB(150, 77, 77, 77));
	FillRBG(x, y, 26, 17, D3DCOLOR_ARGB(200, 0, 0, 0));
	char buf[1024] = { 0 };
	va_list va_alist;
	RECT FontPos = { x, y, x + 120, y + 16 };
	//RECT FontPos = { x, y, x + 150, y + 30 };
	va_start(va_alist, fmt);
	vsprintf_s(buf, fmt, va_alist);
	va_end(va_alist);
	g_pFont->DrawTextA(NULL, buf, -1, &FontPos, DT_NOCLIP, color);
}
void FillRBG(int x, int y, int w, int h, D3DCOLOR color)
{
	D3DRECT rec = { x, y, x + w, y + h };
	d3ddev->Clear(1, &rec, D3DCLEAR_TARGET, color, 0, 0);
}

void DrawLine(float x, float y, float xx, float yy, D3DCOLOR color)
{
	D3DXVECTOR2 dLine[2];

	d3dLine->SetWidth(1.f);

	dLine[0].x = x;
	dLine[0].y = y;

	dLine[1].x = xx;
	dLine[1].y = yy;

	d3dLine->Draw(dLine, 2, color);

}

void DrawBox(float x, float y, float width, float height, D3DCOLOR color)
{
	D3DXVECTOR2 points[5];
	points[0] = D3DXVECTOR2(x, y);
	points[1] = D3DXVECTOR2(x + width, y);
	points[2] = D3DXVECTOR2(x + width, y + height);
	points[3] = D3DXVECTOR2(x, y + height);
	points[4] = D3DXVECTOR2(x, y);
	d3dLine->SetWidth(1);
	d3dLine->Draw(points, 5, color);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		//render();
		break;
	case WM_CREATE:
		DwmExtendFrameIntoClientArea(hWnd, &margin);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
#endif