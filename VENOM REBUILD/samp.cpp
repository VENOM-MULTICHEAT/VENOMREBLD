#include "plugin.h"

DWORD	dwSAMPAddr = 0;
HMODULE hSAMP = 0;
bool bSAMPInit = false;

stSAMP							*g_SAMP = nullptr;
stPlayerPool					*g_Players = nullptr;
stVehiclePool					*g_Vehicles = nullptr;
stChatInfo						*g_Chat = nullptr;
stInputInfo						*g_Input = nullptr;
stKillInfo						*g_DeathList = nullptr;
stScoreboardInfo				*g_Scoreboard = nullptr;
RakClientInterface				*g_RakClient = nullptr;

DWORD Dark::SAMP::getSAMPBase()
{
	return dwSAMPAddr;
}

typedef void(__stdcall *P_destructor_CNetGame)(); P_destructor_CNetGame orig_destructor_CNetGame;
void __stdcall destructor_CNetGame()
{
	Dark::SAMP::Detach();
	return orig_destructor_CNetGame();
}

bool Dark::SAMP::Initialise()
{
	if (bSAMPInit)
		return true;

	if (hSAMP == 0)
		hSAMP = GetModuleHandle("samp.dll");
	if (dwSAMPAddr == 0)
		dwSAMPAddr = (DWORD)hSAMP;

	if (g_SAMP == nullptr)
	{
		g_SAMP = *(stSAMP **)(dwSAMPAddr + SAMP_INFO_OFFSET);
		return false;
	}

	if (g_Chat == nullptr)
	{
		g_Chat = *(stChatInfo **)(dwSAMPAddr + SAMP_CHAT_INFO_OFFSET);
		return false;
	}

	if (g_Input == nullptr)
	{
		g_Input = *(stInputInfo **)(dwSAMPAddr + SAMP_CHAT_INPUT_INFO_OFFSET);
		return false;
	}

	if (g_DeathList == nullptr)
	{
		g_DeathList = *(stKillInfo **)(dwSAMPAddr + SAMP_KILL_INFO_OFFSET);
		return false;
	}

	if (g_Scoreboard == nullptr)
	{
		g_Scoreboard = *(stScoreboardInfo **)(dwSAMPAddr + SAMP_SCOREBOARD_INFO);
		return false;
	}

	if (g_SAMP->pRakClientInterface == nullptr) return false;

	g_RakClient = (RakClientInterface *)g_SAMP->pRakClientInterface;
	g_SAMP->pRakClientInterface = new HookedRakClientInterface();

	g_Vehicles = g_SAMP->pPools->pVehicle;
	g_Players = g_SAMP->pPools->pPlayer;

	orig_destructor_CNetGame = (P_destructor_CNetGame)(dwSAMPAddr + SAMP_FUNC_CNETGAMEDESTRUCTOR);

	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)orig_destructor_CNetGame, destructor_CNetGame);
	DetourTransactionCommit();

	bSAMPInit = true;

	return true;
}

stSAMP *Dark::SAMP::getSAMP()
{
	return g_SAMP;
}

stChatInfo *Dark::SAMP::getChat()
{
	return g_Chat;
}

stInputInfo *Dark::SAMP::getInput()
{
	return g_Input;
}

stKillInfo *Dark::SAMP::getDeathList()
{
	return g_DeathList;
}

stScoreboardInfo *Dark::SAMP::getScoreboard()
{
	return g_Scoreboard;
}

stPlayerPool *Dark::SAMP::getPlayerPool()
{
	return g_Players;
}

stVehiclePool *Dark::SAMP::getVehiclePool()
{
	return g_Vehicles;
}

RakClientInterface *Dark::SAMP::getRakNet()
{
	return g_RakClient;
}

void Dark::SAMP::Detach()
{
	if (g_SAMP->pRakClientInterface != NULL)
		delete g_SAMP->pRakClientInterface;
	g_SAMP->pRakClientInterface = g_RakClient;
	// return ((void(__thiscall *) (void *)) (dwSAMPAddr + SAMP_FUNC_CNETGAMEDESTRUCTOR))(g_SAMP);
}

void Dark::SAMP::ToggleCursor(int iToggle)
{
	void		*obj = *(void **)(dwSAMPAddr + SAMP_MISC_INFO);
	((void(__thiscall *) (void *, int, bool)) (dwSAMPAddr + SAMP_FUNC_TOGGLECURSOR))(obj, iToggle ? 3 : 0, !iToggle);
	if (!iToggle)
		((void(__thiscall *) (void *)) (dwSAMPAddr + SAMP_FUNC_CURSORUNLOCKACTORCAM))(obj);
}

D3DCOLOR Dark::SAMP::getPlayerColor(int id, DWORD trans)
{
	if (dwSAMPAddr == NULL)
		return NULL;

	D3DCOLOR	*color_table;
	if (id < 0 || id >= (SAMP_MAX_PLAYERS + 3))
		return D3DCOLOR_ARGB(0xFF, 0x99, 0x99, 0x99);

	switch (id)
	{
	case (SAMP_MAX_PLAYERS):
		return 0xFF888888;

	case (SAMP_MAX_PLAYERS + 1):
		return 0xFF0000AA;

	case (SAMP_MAX_PLAYERS + 2):
		return 0xFF63C0E2;
	}

	color_table = (D3DCOLOR *)((uint8_t *)dwSAMPAddr + SAMP_COLOR_OFFSET);
	return (color_table[id] >> 8) | trans;
}
