#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <Dwmapi.h> 

//ESP data
extern int s_width;
extern int s_height;
extern int s_widthb;
extern int s_heightb;
#define CENTERX (GetSystemMetrics(SM_CXSCREEN)/2)-(s_width/2)
#define CENTERY (GetSystemMetrics(SM_CYSCREEN)/2)-(s_height/2)
extern LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
extern LPDIRECT3DDEVICE9 d3ddev;
extern HWND hWnd;
extern HWND twnd;
extern MARGINS  margin;
extern LPD3DXFONT pFont;
extern LPD3DXFONT pFont2;
extern ID3DXLine* d3dLine;
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void DrawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char *fmt, ...);
void DrawStringX(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char *fmt, ...);
void DrawLine(float x, float y, float xx, float yy, D3DCOLOR color);
void DrawBox(float x, float y, float width, float height, D3DCOLOR color);
void FillRBG(int x, int y, int w, int h, D3DCOLOR color);
void initD3D(HWND hWnd);
void render();
void DrawBorderString(INT x, INT y, DWORD color, DWORD bordercolor, int borderw, LPD3DXFONT g_pFont, CONST CHAR*fmt, ...);
