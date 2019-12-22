#ifndef VIEW_H_
#define VIEW_H_

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>

class View
{
private :
	static ID3D11Device* g_pd3dDevice;
	static ID3D11DeviceContext* g_pd3dDeviceContext;
	static IDXGISwapChain* g_pSwapChain;
	static ID3D11RenderTargetView* g_mainRenderTargetView;

	static bool CreateDeviceD3D(HWND hWnd);
	static void CleanupDeviceD3D();
	static void CreateRenderTarget();
	static void CleanupRenderTarget();
	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	int Start(int argc, char** argv);
};

#endif // VIEW_H_