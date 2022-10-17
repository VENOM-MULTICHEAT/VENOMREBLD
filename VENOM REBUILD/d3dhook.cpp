#include "plugin.h"

IDirect3DDevice9		*Direct3DDevice9 = NULL;

DWORD FindDevice(DWORD Len)
{
	DWORD dwObjBase = 0;
	char infoBuf[MAX_PATH];
	GetSystemDirectory(infoBuf, MAX_PATH);
	strcat_s(infoBuf, MAX_PATH, "\\d3d9.dll");
	dwObjBase = (DWORD)LoadLibrary(infoBuf);
	while (dwObjBase++ < dwObjBase + Len)
	{
		if ((*(WORD*)(dwObjBase + 0x00)) == 0x06C7 &&
			(*(WORD*)(dwObjBase + 0x06)) == 0x8689 &&
			(*(WORD*)(dwObjBase + 0x0C)) == 0x8689)
		{
			dwObjBase += 2;
			break;
		}
	}
	return(dwObjBase);
};

DWORD GetDeviceAddress(int VTableIndex)
{
	PDWORD VTable;
	*(DWORD*)&VTable = *(DWORD*)FindDevice(0x128000);
	return VTable[VTableIndex];
};

typedef HRESULT(__stdcall *Prototype_Present)(IDirect3DDevice9 *, CONST RECT *, CONST RECT *, HWND, CONST RGNDATA *);
typedef HRESULT(__stdcall *Prototype_Reset)(IDirect3DDevice9 *, D3DPRESENT_PARAMETERS *);

Prototype_Present	Original_Present;
Prototype_Reset		Original_Reset;

bool hook_init = false;
HRESULT WINAPI Hooked_Reset(IDirect3DDevice9 *d3dDevice, D3DPRESENT_PARAMETERS *pPresentationParameters)
{
	if (hook_init)
		reset(false);

	HRESULT result = Original_Reset(d3dDevice, pPresentationParameters);
	if (result == D3D_OK && hook_init)
		reset(true);

	return result;
}

HRESULT WINAPI Hooked_Present(IDirect3DDevice9 *d3dDevice, CONST RECT *pSourceRect, CONST RECT *pDestRect, HWND hDestWindowOverride, CONST RGNDATA *pDirtyRegion)
{
	if (d3dDevice == NULL)
		return Original_Present(d3dDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
	if (Dark::Render::Device() == NULL)
	{
		Dark::Log("Fill Device");
		Dark::Render::Device(d3dDevice);
		Dark::Render::DrawDevice(d3dDevice);
	}

	if (hook_init)
		mainloop();
	else
	{
		Dark::Log(">> Initialise Dear ImGui");
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		// io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
		ImGui_ImplWin32_Init(Dark::Render::getWindow());
		ImGui_ImplDX9_Init(Dark::Render::Device());

		Dark::Log("Dear ImGui Initialised");
		ImGui_Dark_ApplyStyle();
		// ImGui::StyleColorsDark();

		hook_init = true;
	}

	return Original_Present(d3dDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

void Dark::Render::CreateHook()
{
	Dark::Log(">> Find Present");
	Original_Present = (Prototype_Present)GetDeviceAddress(17);
	Dark::Log(">> Find Reset");
	Original_Reset = (Prototype_Reset)GetDeviceAddress(16);

	Dark::Log(">> Install D3D Hooks");

	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)Original_Present, Hooked_Present);
	DetourAttach(&(PVOID&)Original_Reset, Hooked_Reset);
	DetourTransactionCommit();

	Dark::Log("Hooks Installed");
}

void Dark::Render::Detach()
{
	// init_stage = 0;
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&(PVOID&)Original_Present, Hooked_Present);
	DetourDetach(&(PVOID&)Original_Reset, Hooked_Reset);
	DetourTransactionCommit();
}

IDirect3DDevice9 *Dark::Render::Device()
{
	return Direct3DDevice9;
}

IDirect3DDevice9 *Dark::Render::Device(IDirect3DDevice9 *dev)
{
	Direct3DDevice9 = dev;
	return Direct3DDevice9;
}

HWND Dark::Render::getWindow(void)
{
	D3DDEVICE_CREATION_PARAMETERS cparams;
	Dark::Render::Device()->GetCreationParameters(&cparams);
	return cparams.hFocusWindow;
}

POINT2D Dark::Render::getResolution(void)
{
	RECT rect;

	GetWindowRect(Dark::Render::getWindow(), &rect);

	//rect.width  
	//rect.height

	POINT2D p;
	p.x = rect.bottom;
	p.y = rect.right;
	return p;
}
