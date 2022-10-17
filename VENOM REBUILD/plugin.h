#pragma once

#define _CRT_SECURE_NO_WARNINGS
// #define PLUGIN_OUTPUT
// #define PLUGIN_OUTPUT_FILE "DarkSDK.log"

#include <stdio.h> 
#include <stdint.h>
#include <assert.h>
#include <windows.h>
#include <process.h>
#include <fstream>
#include <string.h>
#include <direct.h>

#include <map>
#include <vector>
#include <set>

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define SAFE_RELEASE(punk) if ((punk) != NULL) { (punk)->Release(); (punk) = NULL; }

#include "detours.h"
#pragma comment(lib, "detours.lib")

#include <thread>
#include <WinInet.h>
#pragma comment (lib, "Wininet.lib")

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
#include "DarkStyle.h"

struct D3DVERTEX
{
	float x, y, z, rhw;
	DWORD color;
};

struct POINT2D
{
	int  x;
	int  y;
};

#include "game_sa.h"
#include "samp.h"

#include "BitStream.h"
#include "RakClient.h"
#include "HookedRakClient.h"

#include <dwmapi.h>
#pragma comment(lib, "Dwmapi.lib")

typedef void(__cdecl *InternetCallback)(std::string);

namespace Dark
{
	namespace Render
	{
		void CreateHook(void);

		void DrawTriangle(POINT2D one, POINT2D two, POINT2D three, D3DCOLOR color);
		void DrawBox(int x, int y, int w, int h, D3DCOLOR color);
		void DrawBorderBox(int x, int y, int w, int h, int border, D3DCOLOR color, D3DCOLOR bordercolor);
		void DrawLine(int x1, int y1, int x2, int y2, D3DCOLOR color);
		void DrawCircle(int x, int y, int radius, bool filled, D3DCOLOR color);

		HRESULT LoadFont(LPD3DXFONT &pFont, char* fontname, int height, int weight = 0);
		void Print(LPD3DXFONT &pFont, char *text, int x, int y, int w, int h, bool border, D3DCOLOR color, DWORD format = DT_TOP | DT_LEFT);
		void Print(LPD3DXFONT &pFont, char *text, int x, int y, bool border, D3DCOLOR color);
		int FontWidth(LPD3DXFONT &pFont, char *text);
		int FontHeight(LPD3DXFONT &pFont);

		void Initialise(void);
		void Release(void);
		void ResetFont(LPD3DXFONT &pFont, bool dev);
		void Detach(void);

		POINT2D CursorPos(void);

		IDirect3DDevice9		*Device(void);
		IDirect3DDevice9		*Device(IDirect3DDevice9 *dev);
		IDirect3DDevice9		*DrawDevice(void);
		IDirect3DDevice9		*DrawDevice(IDirect3DDevice9 *dev);
		HWND					getWindow(void);
		POINT2D					getResolution(void);
	};

	namespace Internet
	{
		std::string Sync(std::string url);
		void aSync(InternetCallback v, std::string url);
	};

	namespace GAME
	{
		bool Initialise(void);
		
		CGameSA *getGameInterface(void);
		CPed	*getPlayerPed(void);

		struct actor_info	*getActorInfo(int id, int flags);
		struct vehicle_info *getVehicleInfo(int id, int flags);
	};

	namespace SAMP
	{
		bool Initialise(void);
		void Detach(void);

		DWORD getSAMPBase(void);

		void ToggleCursor(int iToggle);
		D3DCOLOR getPlayerColor(int id, DWORD trans = 0xFF000000);

		stSAMP				*getSAMP(void);
		stPlayerPool		*getPlayerPool(void);
		stVehiclePool		*getVehiclePool(void);
		stChatInfo			*getChat(void);
		stInputInfo			*getInput(void);
		stKillInfo			*getDeathList(void);
		stScoreboardInfo	*getScoreboard(void);
		RakClientInterface	*getRakNet(void);
	};

	namespace VMT
	{
		inline void **get(_In_ void *pThis)
		{
			return *static_cast<void ***>(pThis);
		}
		inline void *get(_In_ void *pThis, _In_opt_ size_t szIndex)
		{
			return get(pThis)[szIndex];
		}
	};

	namespace Key
	{
		void Update(void);
		bool Check(unsigned int key);
		bool CheckJustDown(unsigned int key);
		bool CheckJustUp(unsigned int key);
	};

	void CALLBACK Log(char *text, ...);
	std::string Dir(void);
	bool Match(const char *text, char *buf, const char *begin);
	bool Match(const char *text, char *buf, const char *begin, const char *end);
	void Mod(HMODULE m_Mod);
	HMODULE Mod(void);
};

void mainloop();
void reset(bool dev);

bool OnRPC(int uniqueID, BitStream *parameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp);
bool OnPacket(BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel);
extern std::string ser_key;