#include "plugin.h"
#include <map>


bool		init = false;
WNDPROC		pWndProcOrig;
LPD3DXFONT	pWHFont;

// bug fixes
// added wallhack
// edit new fov zone

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_DESTROY)
	{
		ImGui_ImplDX9_Shutdown();
		ImGui::DestroyContext();
		Dark::Render::Detach();
	}

	ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
	return CallWindowProcA(pWndProcOrig, hwnd, uMsg, wParam, lParam);
}

void reset(bool dev)
{
	if (dev)
	{
		Dark::Render::Initialise();
		ImGui_ImplDX9_CreateDeviceObjects();
	}
	else
	{
		Dark::Render::Release();
		ImGui_ImplDX9_InvalidateDeviceObjects();
	}

	if (!init) return;

	Dark::Render::ResetFont(pWHFont, dev);
}

bool	bMenuState = false;
std::string iAimKey = "X";
int		iBoneId = 0;

int		iAimType = 0;

struct D2D2VEC
{
	D3DXVECTOR3 Orig3D;
	D3DXVECTOR3 Targ3D;
};

std::map< DWORD, D2D2VEC > Tracer;

enum iMode
{
	GLOBAL = 0,
	HEAVY,
	MG,
	RIFLE,
	SHOTGUN,
	SMG,
	Deagle
};

enum PlayerColors // from ARP
{
	Cop = 0xff0000ff,
	Medic = 0xffff6666,
	Groove = 0xff009900,
	News = 0xffff6600,
	CityHall = 0xffccff00,
	Rifa = 0xff6666ff,
	Army = 0xff996633,
	LaCosaNostra = 0xff222222,
	Vagos = 0xffffcd00,
	RussianMafia = 0xff007575,
	Aztec = 0xff00ccff,
	Ballas = 0xffcc00ff
};

DWORD dwPlayerColors[12] = {
	0xff0000ff,
	0xffff6666,
	0xff009900,
	0xffff6600,
	0xffccff00,
	0xff6666ff,
	0xff996633,
	0xff222222,
	0xffffcd00,
	0xff007575,
	0xff00ccff,
	0xffcc00ff
};

int iFractionMode = 0;
bool bFractionEnable[12] = { 0 };

bool	bEnabled[6] = { 0, 0, 0, 0, 0, 0 };
int		iCurrentMode = iMode::GLOBAL;
int		iCurrentModeEV = iMode::GLOBAL;
int		iCurrentFOV[7] = { 50, 50, 50, 50, 50, 50, 50 };
float	fSmooth[7] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
int		iSmoothPercent[7] = { 1, 1, 1, 1, 1, 1, 1 };
int		iSpeed[7] = { 1, 1, 1, 1, 1, 1, 1 };
int		iTargetPlayer = -1;

float	fSafeZone[7] = { 0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f };

bool	bAutoShot, bAutoShotDown = false;
bool	bNoSpread = false;

int XType = 0;
bool XOnShoot = false;
int XModeBone = 0;
int XBone = 0;

bool Silent = false;
int chance = 100;
bool bIngnoreT = false;
bool capture = false;

bool SearchFile(std::string path)
{
	FILE *file;
	bool res;
	file = fopen(path.c_str(), "r");
	res = (file ? true : false);
	fclose(file);
	return res;
}

CPed *getPed(int playerId, bool onscreen = true)
{
	if (Dark::SAMP::getPlayerPool()->iIsListed[playerId] != 1)
		return NULL;
	if (Dark::SAMP::getPlayerPool()->pRemotePlayer[playerId] == NULL)
		return NULL;
	if (Dark::SAMP::getPlayerPool()->pRemotePlayer[playerId]->pPlayerData == NULL)
		return NULL;
	if (Dark::SAMP::getPlayerPool()->pRemotePlayer[playerId]->pPlayerData->pSAMP_Actor == NULL)
		return NULL;
	if (Dark::SAMP::getPlayerPool()->pRemotePlayer[playerId]->pPlayerData->pSAMP_Vehicle != NULL)
		return NULL;

	actor_info *actor = Dark::SAMP::getPlayerPool()->pRemotePlayer[playerId]->pPlayerData->pSAMP_Actor->pGTA_Ped;
	CPed *ped = Dark::GAME::getGameInterface()->GetPools()->AddPed((DWORD *)actor);

	if (onscreen && !ped->IsOnScreen())
		return NULL;

	return ped;
}

std::wstring get_utf16(const std::string &str, int codepage)
{
	if (str.empty()) return std::wstring();
	int sz = MultiByteToWideChar(codepage, 0, &str[0], (int)str.size(), 0, 0);
	std::wstring res(sz, 0);
	MultiByteToWideChar(codepage, 0, &str[0], (int)str.size(), &res[0], sz);
	return res;
}

void CalcScreenCoors(D3DXVECTOR3 *vecWorld, D3DXVECTOR3 *vecScreen)
{
	/** C++-ifyed function 0x71DA00, formerly called by CHudSA::CalcScreenCoors **/
	// Get the static view matrix as D3DXMATRIX
	D3DXMATRIX	m((float *)(0xB6FA2C));

	// Get the static virtual screen (x,y)-sizes
	DWORD		*dwLenX = (DWORD *)(0xC17044);
	DWORD		*dwLenY = (DWORD *)(0xC17048);

	//DWORD *dwLenZ = (DWORD*)(0xC1704C);
	//double aspectRatio = (*dwLenX) / (*dwLenY);
	// Do a transformation
	vecScreen->x = (vecWorld->z * m._31) + (vecWorld->y * m._21) + (vecWorld->x * m._11) + m._41;
	vecScreen->y = (vecWorld->z * m._32) + (vecWorld->y * m._22) + (vecWorld->x * m._12) + m._42;
	vecScreen->z = (vecWorld->z * m._33) + (vecWorld->y * m._23) + (vecWorld->x * m._13) + m._43;

	// Get the correct screen coordinates
	double	fRecip = (double)1.0 / vecScreen->z;	//(vecScreen->z - (*dwLenZ));
	vecScreen->x *= (float)(fRecip * (*dwLenX));
	vecScreen->y *= (float)(fRecip * (*dwLenY));
}

struct BoneX { CVector x3D; CVector2D x2D; };
bool getBonePosition(CPed *ped, eBone bone, BoneX &Out)
{
	CVector BonePos; ped->GetTransformedBonePosition(bone, &BonePos);
	D3DXVECTOR3 BonePosX, BonePosX2D;
	BonePosX.x = BonePos.fX;
	BonePosX.y = BonePos.fY;
	BonePosX.z = BonePos.fZ;
	CalcScreenCoors(&BonePosX, &BonePosX2D);
	Out.x3D = BonePos;
	Out.x2D.fX = BonePosX2D.x;
	Out.x2D.fY = BonePosX2D.y;
	return TRUE;
}

POINT2D GetCrosshairPosition()
{
	return
	{
		(int)(GetSystemMetrics(SM_CXSCREEN) * 0.5299999714f),
		(int)(GetSystemMetrics(SM_CYSCREEN) * 0.4f)
	};
}

float CalcScreenDist(float x1, float y1, float x2, float y2)
{
	float x = x2 - x1; x = x * x;
	float y = y2 - y1; y = y * y;
	return sqrt(x + y);
}

int FindNearestBone(CPed *ped)
{
	// 0 = BONE_HEAD
	// 1 = BONE_LEFTHAND
	// 2 = BONE_RIGHTHAND
	// 3 = BONE_LEFTFOOT
	// 4 = BONE_RIGHTFOOT
	// 5 = BONE_SPINE1
	// 6 = BONE_PELVIS

	POINT2D Crosshair = GetCrosshairPosition();

	int NearestBoneSaved;
	float DistSaved = 0.0f;

	for (int i = 0; i <= 6; i++)
	{
		eBone pGtaBone;

		switch (i)
		{
		case 0:
		{
			pGtaBone = BONE_HEAD;
			break;
		}
		case 1:
		{
			pGtaBone = BONE_LEFTHAND;
			break;
		}
		case 2:
		{
			pGtaBone = BONE_RIGHTHAND;
			break;
		}
		case 3:
		{
			pGtaBone = BONE_LEFTFOOT;
			break;
		}
		case 4:
		{
			pGtaBone = BONE_RIGHTFOOT;
			break;
		}
		case 5:
		{
			pGtaBone = BONE_SPINE1;
			break;
		}
		case 6:
		{
			pGtaBone = BONE_PELVIS;
			break;
		}
		default:
			break;
		}

		BoneX Bone;
		getBonePosition(ped, pGtaBone, Bone);
		float dst = abs(CalcScreenDist(Crosshair.x, Crosshair.y, Bone.x2D.fX, Bone.x2D.fY));

		if (DistSaved == 0.0f || dst < DistSaved)
		{
			DistSaved = dst;
			NearestBoneSaved = i;
		}
	}

	return NearestBoneSaved;
}

float randf(float min, float max)
{
	float result = min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
	return result;
}

bool OnRPC(int uniqueID, BitStream *parameters, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp)
{
	return true;
}

bool RendersBT = false;

bool OnPacket(BitStream * bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
	if (bitStream == nullptr) return true;

	byte packetId;
	bitStream->ResetReadPointer();
	bitStream->Read(packetId);
	bitStream->ResetReadPointer();

	if (packetId == ID_BULLET_SYNC)
	{
		stBulletData bulletData = {};
		bitStream->ResetReadPointer();
		bitStream->IgnoreBits(8);
		bitStream->Read((char *)&bulletData, sizeof(stBulletData));
		bitStream->ResetReadPointer();

		if (RendersBT)
		{
			D3DXVECTOR3 OriginX;
			D3DXVECTOR3 TargetX;

			OriginX.x = bulletData.fOrigin[0];
			OriginX.y = bulletData.fOrigin[1];
			OriginX.z = bulletData.fOrigin[2];

			if (bNoSpread == true)
			{
				TargetX.x = bulletData.fTarget[0] + randf(-4.0f, 4.0f);
				TargetX.y = bulletData.fTarget[1] + randf(-4.0f, 4.0f);
			}
			else
			{
				TargetX.x = bulletData.fTarget[0];
				TargetX.y = bulletData.fTarget[1];
			}
			TargetX.z = bulletData.fTarget[2];

			D2D2VEC vec;
			vec.Orig3D = OriginX;
			vec.Targ3D = TargetX;
			Tracer.insert( std::pair<DWORD, D2D2VEC>(GetTickCount() + 5000, vec) );
		}

		if (iTargetPlayer == -1)
			return true;

		if (iFractionMode != 0)
		{
			DWORD color = Dark::SAMP::getPlayerColor(iTargetPlayer);

			bool bSearched = false;
			for (int c = 0; c < 12; c++)
			{
				if (color == dwPlayerColors[c])
					bSearched = true;
			}

			if (iFractionMode == 1 && bSearched)
				return true;
			if (iFractionMode == 2 && !bSearched)
				return true;
		}

		if (XType == 2)
			XOnShoot = true;

		if (Silent)
		{
			if (chance != 100)
			{
				if (rand() % 100 > chance)
					return true;
			}

			CPed *ped = getPed(iTargetPlayer);
			if (ped == NULL)
				return true;

			eBone pGtaBone;
			int iXBoneId = iBoneId;

			if (XModeBone == 1)
				iXBoneId = FindNearestBone(ped);
			if (XModeBone == 2)
				iXBoneId = rand() % 6;

			switch (iXBoneId)
			{
			case 0:
			{
				pGtaBone = BONE_HEAD;
				break;
			}
			case 1:
			{
				pGtaBone = BONE_LEFTHAND;
				break;
			}
			case 2:
			{
				pGtaBone = BONE_RIGHTHAND;
				break;
			}
			case 3:
			{
				pGtaBone = BONE_LEFTFOOT;
				break;
			}
			case 4:
			{
				pGtaBone = BONE_RIGHTFOOT;
				break;
			}
			case 5:
			{
				pGtaBone = BONE_SPINE1;
				break;
			}
			case 6:
			{
				pGtaBone = BONE_PELVIS;
				break;
			}
			default:
				break;
			}

			BoneX Bone; getBonePosition(ped, pGtaBone, Bone);
			int weapon_id = Dark::SAMP::getPlayerPool()->pLocalPlayer->byteCurrentWeapon;

			stBulletData sync;
			ZeroMemory(&sync, sizeof(stBulletData));
			sync.byteType = 1;
			sync.byteWeaponID = weapon_id;
			sync.fCenter[0] = randf(-0.01f, 0.01f);
			sync.fCenter[1] = randf(-0.01f, 0.01f);
			sync.fCenter[2] = randf(0.3f, 0.6f);
			sync.fOrigin[0] = Dark::GAME::getPlayerPed()->GetPosition()->fX;
			sync.fOrigin[1] = Dark::GAME::getPlayerPed()->GetPosition()->fY;
			sync.fOrigin[2] = Dark::GAME::getPlayerPed()->GetPosition()->fZ;
			sync.fTarget[0] = Bone.x3D.fX;
			sync.fTarget[1] = Bone.x3D.fY;
			sync.fTarget[2] = Bone.x3D.fZ;
			sync.sTargetID = iTargetPlayer;
			bitStream->SetWriteOffset(0);
			bitStream->Write(packetId);
			bitStream->Write((PCHAR)&sync, sizeof(stBulletData));
		}
	}

	return true;
}

VOID __stdcall KeyEvent()
{
	mouse_event(MOUSEEVENTF_LEFTDOWN, NULL, NULL, NULL, NULL);
	Sleep(100);
	mouse_event(MOUSEEVENTF_LEFTUP, NULL, NULL, NULL, NULL);
	_endthread();
}

VOID api_KeyEvent(int mode) 
{ 
	if (mode == 0 && bAutoShotDown == false)
	{
		mouse_event(MOUSEEVENTF_LEFTDOWN, NULL, NULL, NULL, NULL);
		bAutoShotDown = true;
	}
	else if (mode == 1)
	{
		mouse_event(MOUSEEVENTF_LEFTUP, NULL, NULL, NULL, NULL);
		bAutoShotDown = false;
	}
	else if (mode == 2)
		std::thread(KeyEvent).detach(); 
}

float cheat_AimEvent(float orig, float to)
{
	if (iAimType == 0) return orig;
	if (iAimType == 1) return to;

	float abs_orig = abs(orig);
	float abs_to = abs(to);
	float abs_dst = abs(abs_orig - abs_to);
	float offset = 0.0F;

	if (abs_dst > fSafeZone[iCurrentModeEV])
	{
		if (iAimType == 2) offset = ((float)iSpeed[iCurrentModeEV] / 1000.0f);
		if (iAimType == 3) offset = abs_dst / fSmooth[iCurrentModeEV];
		if (iAimType == 4) offset = (abs_dst * iSmoothPercent[iCurrentModeEV]) / 100;

		if (orig < 0.0F) offset = -offset;

		if (abs_orig > abs_to)	orig -= offset;
		else					orig += offset;
	}

	return orig;
}

struct CamOffset { float Alpha, Beta; };
CamOffset getCamOffset(CPed *Ped, eBone _Bone)
{
	CVector			Bone; Ped->GetTransformedBonePosition(_Bone, &Bone);

	float				ax, az, fz, fx;
	CVector				vect;
	float				*screenAspectRatio = (float *)0xC3EFA4;
	float				*crosshairOffset = (float *)0xB6EC10;

	CCamera *pCamera = pGame->GetCamera();
	CCamSAInterface *pCam = (CCamSAInterface *)((CCamSA *)pCamera->GetCam(pCamera->GetActiveCam()))->GetInterface();

	vect = pCam->Source - Bone;

	if (pCam->Mode == 53 || pCam->Mode == 55) // 3rd person mode
	{
		// weird shit
		float mult = tan(pCam->FOV * 0.5f * 0.017453292f);
		fz = M_PI - atan2(1.0f, mult * ((0.5f - crosshairOffset[0] + 0.5f - crosshairOffset[0]) * (1.0f / *screenAspectRatio)));
		// fx = M_PI - atan2(1.0f, mult * ((0.5f - crosshairOffset[1] + 0.5f - crosshairOffset[1]) * (1.0f / *screenAspectRatio)));
		fx = M_PI - atan2(1.0f, mult * (crosshairOffset[1] - 0.5f + crosshairOffset[1] - 0.5f));
	}
	else
	{
		fx = fz = M_PI / 2;
	}
	// x angle
	float dist = sqrt(vect.fX * vect.fX + vect.fY * vect.fY);
	az = atan2f(dist, vect.fZ);

	// z angle
	ax = atan2f(vect.fY, -vect.fX) - M_PI / 2;

	return { (az - fz), -(ax - fx) };
}

bool RendersLines = false;
bool RendersCircle = false;
bool RendersFOV = false;
bool RendersBone = false;
bool RendersBoxes = false;
bool RendersPN = false;

void ParseVec(D3DXVECTOR3 src, CVector &out)
{
	out.fX = src.x;
	out.fY = src.y;
	out.fZ = src.z;
}

int FovEV(int n, int to, int q = 2)
{

	int dst = abs(n - to);
	if (dst > q)
	{
		if (n > to) n -= q;
		else n += q;
	}
	else n = to;
	return n;
}

POINT2D LastFovZone;
int LastFovSize = 0;

void RendersLoop(void)
{
	if (RendersFOV)
	{
		if (iTargetPlayer == -1)
		{
			POINT2D CurrentFovZone = GetCrosshairPosition();
			LastFovZone.x = FovEV(LastFovZone.x, CurrentFovZone.x, 4);
			LastFovZone.y = FovEV(LastFovZone.y, CurrentFovZone.y, 4);
			LastFovSize = FovEV(LastFovSize, (iCurrentFOV[iCurrentModeEV] / 2), 4);
		}
		else
		{
			CPed *Ped = getPed(iTargetPlayer);
			if (Ped == NULL)
			{
				POINT2D CurrentFovZone = GetCrosshairPosition();
				LastFovZone.x = FovEV(LastFovZone.x, CurrentFovZone.x, 4);
				LastFovZone.y = FovEV(LastFovZone.y, CurrentFovZone.y, 4);
			}
			else
			{
				BoneX center; getBonePosition(Ped, eBone::BONE_SPINE1, center);

				CCamera *pCamera = pGame->GetCamera();
				CCamSAInterface *pCam = (CCamSAInterface *)((CCamSA *)pCamera->GetCam(pCamera->GetActiveCam()))->GetInterface();
				CVector src = pCam->Source;
				if (Dark::GAME::getGameInterface()->GetWorld()->IsLineOfSightClear(&src, &center.x3D, true, false, false, true, true, false, false) == FALSE)
				{
					POINT2D CurrentFovZone = GetCrosshairPosition();
					LastFovZone.x = FovEV(LastFovZone.x, CurrentFovZone.x, 4);
					LastFovZone.y = FovEV(LastFovZone.y, CurrentFovZone.y, 4);
				}
				else
				{
					LastFovZone.x = FovEV(LastFovZone.x, center.x2D.fX, 4);
					LastFovZone.y = FovEV(LastFovZone.y, center.x2D.fY, 4);
					LastFovSize = FovEV(LastFovSize, 10, 4);
				}
			}
		}

		Dark::Render::DrawCircle(LastFovZone.x, LastFovZone.y, LastFovSize, false, 0xffff4040);
	}

	if (RendersBT)
	{
		for (auto it = Tracer.begin(); it != Tracer.end(); ++it)
		{
			if (GetTickCount() > it->first)
				Tracer.erase(it);
			else
			{
				D3DXVECTOR3 Orig, Targ, Orig3D, Targ3D;
				Orig3D = it->second.Orig3D;
				Targ3D = it->second.Targ3D;
				CalcScreenCoors(&Orig3D, &Orig);
				CalcScreenCoors(&Targ3D, &Targ);

				CVector vecOrig3D, vecTarg3D;
				ParseVec(Orig3D, vecOrig3D);
				ParseVec(Targ3D, vecTarg3D);

				CCamera *pCamera = pGame->GetCamera();
				CCamSAInterface *pCam = (CCamSAInterface *)((CCamSA *)pCamera->GetCam(pCamera->GetActiveCam()))->GetInterface();
				CVector src = pCam->Source;
				if (Dark::GAME::getGameInterface()->GetWorld()->IsLineOfSightClear(&src, &vecOrig3D, true, false, false, true, true, false, false) == FALSE)
					continue;
				if (Dark::GAME::getGameInterface()->GetWorld()->IsLineOfSightClear(&src, &vecTarg3D, true, false, false, true, true, false, false) == FALSE)
					continue;

				D3DCOLOR Color = Dark::SAMP::getPlayerColor(Dark::SAMP::getPlayerPool()->sLocalPlayerID);
				Dark::Render::DrawLine(Orig.x, Orig.y, Targ.x, Targ.y, Color);
				Dark::Render::DrawCircle(Targ.x, Targ.y, 5, false, Color);
			}
		}
	}

	if (RendersLines ||
		RendersCircle ||
		RendersBone ||
		RendersBoxes ||
		RendersPN)
	{
		for (int i = 0; i <= SAMP_MAX_PLAYERS; i++)
		{
			CPed *Ped = getPed(i);
			if (Ped == NULL)
				continue;

			D3DCOLOR color = Dark::SAMP::getPlayerColor(i);

			BoneX xB; getBonePosition(Ped, eBone::BONE_SPINE1, xB);
			BoneX selfxB; getBonePosition(Dark::GAME::getPlayerPed(), eBone::BONE_SPINE1, selfxB);

			if (RendersLines)
				Dark::Render::DrawLine(selfxB.x2D.fX, selfxB.x2D.fY, xB.x2D.fX, xB.x2D.fY, color);
			if (RendersCircle)
				Dark::Render::DrawCircle(xB.x2D.fX, xB.x2D.fY, 10, false, color);
			if (RendersBone)
			{
				BoneX b1; getBonePosition(Ped, (eBone)54, b1);
				BoneX b2; getBonePosition(Ped, (eBone)53, b2);
				Dark::Render::DrawLine(b1.x2D.fX, b1.x2D.fY, b2.x2D.fX, b2.x2D.fY, color);

				getBonePosition(Ped, (eBone)52, b1);
				Dark::Render::DrawLine(b2.x2D.fX, b2.x2D.fY, b1.x2D.fX, b1.x2D.fY, color);

				getBonePosition(Ped, (eBone)51, b2);
				Dark::Render::DrawLine(b1.x2D.fX, b1.x2D.fY, b2.x2D.fX, b2.x2D.fY, color);

				getBonePosition(Ped, (eBone)1, b1);
				Dark::Render::DrawLine(b2.x2D.fX, b2.x2D.fY, b1.x2D.fX, b1.x2D.fY, color);

				getBonePosition(Ped, (eBone)41, b2);
				Dark::Render::DrawLine(b1.x2D.fX, b1.x2D.fY, b2.x2D.fX, b2.x2D.fY, color);

				getBonePosition(Ped, (eBone)42, b1);
				Dark::Render::DrawLine(b2.x2D.fX, b2.x2D.fY, b1.x2D.fX, b1.x2D.fY, color);

				getBonePosition(Ped, (eBone)43, b2);
				Dark::Render::DrawLine(b1.x2D.fX, b1.x2D.fY, b2.x2D.fX, b2.x2D.fY, color);

				getBonePosition(Ped, (eBone)44, b1);
				Dark::Render::DrawLine(b2.x2D.fX, b2.x2D.fY, b1.x2D.fX, b1.x2D.fY, color);

				///////////////////////////////

				getBonePosition(Ped, (eBone)1, b1);
				getBonePosition(Ped, (eBone)2, b2);
				Dark::Render::DrawLine(b1.x2D.fX, b1.x2D.fY, b2.x2D.fX, b2.x2D.fY, color);

				getBonePosition(Ped, (eBone)3, b1);
				Dark::Render::DrawLine(b2.x2D.fX, b2.x2D.fY, b1.x2D.fX, b1.x2D.fY, color);

				getBonePosition(Ped, (eBone)4, b2);
				Dark::Render::DrawLine(b1.x2D.fX, b1.x2D.fY, b2.x2D.fX, b2.x2D.fY, color);

				getBonePosition(Ped, (eBone)21, b1);
				Dark::Render::DrawLine(b2.x2D.fX, b2.x2D.fY, b1.x2D.fX, b1.x2D.fY, color);

				getBonePosition(Ped, (eBone)22, b2);
				Dark::Render::DrawLine(b1.x2D.fX, b1.x2D.fY, b2.x2D.fX, b2.x2D.fY, color);

				getBonePosition(Ped, (eBone)23, b1);
				Dark::Render::DrawLine(b2.x2D.fX, b2.x2D.fY, b1.x2D.fX, b1.x2D.fY, color);

				getBonePosition(Ped, (eBone)24, b2);
				Dark::Render::DrawLine(b1.x2D.fX, b1.x2D.fY, b2.x2D.fX, b2.x2D.fY, color);

				getBonePosition(Ped, (eBone)4, b1);
				getBonePosition(Ped, (eBone)31, b2);
				Dark::Render::DrawLine(b1.x2D.fX, b1.x2D.fY, b2.x2D.fX, b2.x2D.fY, color);

				getBonePosition(Ped, (eBone)31, b1);
				Dark::Render::DrawLine(b2.x2D.fX, b2.x2D.fY, b1.x2D.fX, b1.x2D.fY, color);

				getBonePosition(Ped, (eBone)32, b2);
				Dark::Render::DrawLine(b1.x2D.fX, b1.x2D.fY, b2.x2D.fX, b2.x2D.fY, color);

				getBonePosition(Ped, (eBone)33, b1);
				Dark::Render::DrawLine(b2.x2D.fX, b2.x2D.fY, b1.x2D.fX, b1.x2D.fY, color);

				getBonePosition(Ped, (eBone)34, b2);
				Dark::Render::DrawLine(b1.x2D.fX, b1.x2D.fY, b2.x2D.fX, b2.x2D.fY, color);

				///////////////////////////////

				getBonePosition(Ped, (eBone)4, b1);
				getBonePosition(Ped, (eBone)5, b2);
				Dark::Render::DrawLine(b1.x2D.fX, b1.x2D.fY, b2.x2D.fX, b2.x2D.fY, color);

				getBonePosition(Ped, (eBone)8, b1);
				Dark::Render::DrawLine(b2.x2D.fX, b2.x2D.fY, b1.x2D.fX, b1.x2D.fY, color);
			}
			if (RendersBoxes)
			{
				BoneX Left;  getBonePosition(Ped, (eBone)22, Left);
				BoneX Right;  getBonePosition(Ped, (eBone)32, Right);
				BoneX Top;  getBonePosition(Ped, (eBone)6, Top);
				BoneX Bottom;  getBonePosition(Ped, (eBone)54, Bottom);

				Dark::Render::DrawLine(Left.x2D.fX, Top.x2D.fY, Right.x2D.fX, Top.x2D.fY, color);
				Dark::Render::DrawLine(Left.x2D.fX, Bottom.x2D.fY, Right.x2D.fX, Bottom.x2D.fY, color);
				
				Dark::Render::DrawLine(Left.x2D.fX, Top.x2D.fY, Left.x2D.fX, Bottom.x2D.fY, color);
				Dark::Render::DrawLine(Right.x2D.fX, Top.x2D.fY, Right.x2D.fX, Bottom.x2D.fY, color);
			}
			if (RendersPN)
			{
				BoneX Head; getBonePosition(Ped, eBone::BONE_HEAD, Head);
				Head.x2D.fY += 5.0f;

				char buffer[25];
				ZeroMemory(buffer, sizeof(buffer));
				sprintf(buffer, Dark::SAMP::getPlayerPool()->pRemotePlayer[i]->strPlayerName.c_str());

				int hp = static_cast<int>(Dark::SAMP::getPlayerPool()->pRemotePlayer[i]->pPlayerData->fActorHealth);
				Dark::Render::Print(pWHFont, buffer, 
					Head.x2D.fX, Head.x2D.fY, true, color);
				Dark::Render::DrawBox(Head.x2D.fX, Head.x2D.fY + Dark::Render::FontHeight(pWHFont) + 2, hp, 3, D3DCOLOR_ARGB(255, 255, 50, 50));
			}
		}
	}
}

std::string ini = "";

void WriteIniString(std::string sect, std::string key, std::string str)
{
	WritePrivateProfileStringA(sect.c_str(), key.c_str(), str.c_str(), ini.c_str());
}

void WriteIniInt(std::string sect, std::string key, int i)
{
	WritePrivateProfileStringA(sect.c_str(), key.c_str(), std::to_string(i).c_str(), ini.c_str());
}

void WriteIniFloat(std::string sect, std::string key, float f)
{
	WritePrivateProfileStringA(sect.c_str(), key.c_str(), std::to_string(f).c_str(), ini.c_str());
}

void WriteIniBool(std::string sect, std::string key, bool b)
{
	std::string str = "false";
	if (b) str = "true";

	WritePrivateProfileStringA(sect.c_str(), key.c_str(), str.c_str(), ini.c_str());
}

std::string ReadIniString(std::string sect, std::string key)
{
	char buffer[256];
	ZeroMemory(buffer, sizeof(buffer));
	GetPrivateProfileStringA(sect.c_str(), key.c_str(), "", buffer, sizeof(buffer), ini.c_str());
	return std::string(buffer);
}

int ReadIniInt(std::string sect, std::string key)
{
	char buffer[256];
	ZeroMemory(buffer, sizeof(buffer));
	GetPrivateProfileStringA(sect.c_str(), key.c_str(), "", buffer, sizeof(buffer), ini.c_str());
	int i;
	sscanf(buffer, "%d", &i);
	return i;
}

float ReadIniFloat(std::string sect, std::string key)
{
	char buffer[256];
	ZeroMemory(buffer, sizeof(buffer));
	GetPrivateProfileStringA(sect.c_str(), key.c_str(), "", buffer, sizeof(buffer), ini.c_str());
	float f;
	sscanf(buffer, "%f", &f);
	return f;
}

bool ReadIniBool(std::string sect, std::string key)
{
	char buffer[256];
	bool b = false;
	ZeroMemory(buffer, sizeof(buffer));
	GetPrivateProfileStringA(sect.c_str(), key.c_str(), "", buffer, sizeof(buffer), ini.c_str());
	if (std::string(buffer) == "true")
		b = true;
	return b;
}

void WriteIniInfo()
{
	WriteIniString("Global", "Key", iAimKey);
	WriteIniInt("Global", "AimType", iAimType);
	WriteIniInt("Global", "XOpt", XType);
	WriteIniInt("Global", "Bone", iBoneId);
	WriteIniInt("Global", "Chance", chance);
	WriteIniInt("Global", "Mode Bone", XModeBone);
	WriteIniBool("Global", "Ignore My Team", bIngnoreT);

	WriteIniInt("Fraction", "Mode", iFractionMode);
	for (int i = 0; i < 12; i++)
		WriteIniBool("Fraction", ("F" + std::to_string(i)).c_str(), bFractionEnable[i]);

	for (int i = 0; i < 6; i++)
		WriteIniBool("Universal", ("State ( " + std::to_string(i) + " )").c_str(), bEnabled[i]);

	for (int i = 0; i < 7; i++)
		WriteIniInt("Universal", ("FOV ( " + std::to_string(i) + " )").c_str(), iCurrentFOV[i]);

	for (int i = 0; i < 7; i++)
		WriteIniFloat("Universal", ("Smooth ( " + std::to_string(i) + " )").c_str(), fSmooth[i]);

	for (int i = 0; i < 7; i++)
		WriteIniInt("Universal", ("Smooth Percent ( " + std::to_string(i) + " )").c_str(), iSmoothPercent[i]);

	for (int i = 0; i < 7; i++)
		WriteIniInt("Universal", ("Speed ( " + std::to_string(i) + " )").c_str(), iSpeed[i]);

	for (int i = 0; i < 7; i++)
		WriteIniFloat("Universal", ("Safe Zone ( " + std::to_string(i) + " )").c_str(), fSafeZone[i]);
}

void ReadIniInfo()
{
	std::string prev = ReadIniString("Global", "Key");
	if (prev == "")
		WriteIniInfo();
	iAimKey = ReadIniString("Global", "Key");

	iAimType = ReadIniInt("Global", "AimType");
	XType = ReadIniInt("Global", "XOpt");
	iBoneId = ReadIniInt("Global", "Bone");
	chance = ReadIniInt("Global", "Chance");
	XModeBone = ReadIniInt("Global", "Mode Bone");
	bIngnoreT = ReadIniBool("Global", "Ignore My Team");

	iFractionMode = ReadIniInt("Fraction", "Mode");
	for (int i = 0; i < 12; i++)
		bFractionEnable[i] = ReadIniBool("Fraction", ("F" + std::to_string(i)).c_str());

	for (int i = 0; i < 6; i++)
		bEnabled[i] = ReadIniBool("Universal", ("State ( " + std::to_string(i) + " )").c_str());

	for (int i = 0; i < 7; i++)
		iCurrentFOV[i] = ReadIniInt("Universal", ("FOV ( " + std::to_string(i) + " )").c_str());

	for (int i = 0; i < 7; i++)
		fSmooth[i] = ReadIniFloat("Universal", ("Smooth ( " + std::to_string(i) + " )").c_str());

	for (int i = 0; i < 7; i++)
		iSmoothPercent[i] = ReadIniInt("Universal", ("Smooth Percent ( " + std::to_string(i) + " )").c_str());

	for (int i = 0; i < 7; i++)
		iSpeed[i] = ReadIniInt("Universal", ("Speed ( " + std::to_string(i) + " )").c_str());

	for (int i = 0; i < 7; i++)
		fSafeZone[i] = ReadIniFloat("Universal", ("Safe Zone ( " + std::to_string(i) + " )").c_str());

}

DWORD lpFreeLibrary(void *arg)
{
	SetWindowLongA(Dark::Render::getWindow(), GWL_WNDPROC, LONG(pWndProcOrig));
	Dark::Render::Release();
	Dark::Render::Detach();
	Dark::Render::Device(NULL);
	ImGui_ImplDX9_Shutdown();
	ImGui::DestroyContext();
	// FreeLibrary(Dark::Mod());
	return 1;
}

DWORD lpAndHook(void *arg)
{
	Dark::Render::Release();
	Dark::Render::Detach();
	Dark::Render::Device(NULL);
	// ImGui_ImplDX9_Shutdown();
	// ImGui::DestroyContext();
	Dark::Render::CreateHook();
	// FreeLibrary(Dark::Mod());
	return 1;
}

DWORD lpFreeCheat(void *arg)
{
	Dark::Render::Release();
	Dark::Render::Detach();
	Dark::Render::Device(NULL);
	ImGui_ImplDX9_Shutdown();
	ImGui::DestroyContext();
	terminate();
	// FreeLibrary(Dark::Mod());
	return 1;
}

void GetVolumeID(std::string& VolumeID)
{
	DWORD Volume;
	::GetVolumeInformation("c:\\", NULL, 12, &Volume, NULL, NULL, NULL, 10);

	char szText[64];
	_itoa_s(Volume, szText, 16);
	VolumeID = szText;
}

void mainloop()
{
	Dark::Key::Update();

	if (init == false)
	{
		if (!Dark::GAME::Initialise()) return;
		// if (Dark::GAME::getGameInterface() == nullptr) return;
		// if (Dark::GAME::getPlayerPed() == nullptr) return;
		// if (Dark::GAME::getGameInterface()->GetSystemState() != eSystemState::GS_PLAYING_GAME) return;
		if (!Dark::SAMP::Initialise()) return;

		// тут защита
		std::string key; GetVolumeID(key);

		//std::string reason = Dark::Internet::Sync("https://pohkakoy.000webhostapp.com/io" + key);

		/*
		if (key != "keykode") // native check 
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)lpFreeCheat, NULL, NULL, NULL);
		*/
		/*
		if (reason != "SUCCEEDED") // server check
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)lpFreeCheat, NULL, NULL, NULL);
		*/

		Dark::Log("Init complete");

		pWndProcOrig = (WNDPROC)SetWindowLongA(Dark::Render::getWindow(), GWL_WNDPROC, LONG(WindowProc));
		Dark::Render::Initialise();
		Dark::Render::LoadFont(pWHFont, "Arial", 15);

		ini = Dark::Dir() + "\\config.ini";
		ReadIniInfo();

		LastFovZone = GetCrosshairPosition();
		LastFovSize = (iCurrentFOV[iCurrentModeEV] / 2);

		init = true;
	}
	
	if (Dark::GAME::getPlayerPed() != NULL && Dark::GAME::getPlayerPed()->IsOnScreen())
	{
		eWeaponSlot slot = Dark::GAME::getPlayerPed()->GetCurrentWeaponSlot();
		if (bEnabled[0] && slot == eWeaponSlot::WEAPONSLOT_TYPE_HEAVY)
			iCurrentModeEV = iMode::HEAVY;
		else if (bEnabled[1] && slot == eWeaponSlot::WEAPONSLOT_TYPE_MG)
			iCurrentModeEV = iMode::MG;
		else if (bEnabled[2] && slot == eWeaponSlot::WEAPONSLOT_TYPE_RIFLE)
			iCurrentModeEV = iMode::RIFLE;
		else if (bEnabled[3] && slot == eWeaponSlot::WEAPONSLOT_TYPE_SHOTGUN)
			iCurrentModeEV = iMode::SHOTGUN;
		else if (bEnabled[4] && slot == eWeaponSlot::WEAPONSLOT_TYPE_SMG)
			iCurrentModeEV = iMode::SMG;
		else if (bEnabled[5] && Dark::SAMP::getPlayerPool()->pLocalPlayer->byteCurrentWeapon == 24)
			iCurrentModeEV = iMode::Deagle;
		else
			iCurrentModeEV = iMode::GLOBAL;
	}

	if (bMenuState)
	{
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// ImGui_Rose_ShowStyleEditor();

		ImGui::Begin("AIM MENU", &bMenuState, ImGuiWindowFlags_NoSavedSettings);
		
		if (ImGui::CollapsingHeader("AIMS"))
		{
			ImGui::Text("Aim Type: ");
			ImGui::RadioButton("None##AimType", &iAimType, 0);
			ImGui::RadioButton("Default##AimType", &iAimType, 1);
			ImGui::RadioButton("Fixed##AimType", &iAimType, 2);
			ImGui::RadioButton("Smooth##AimType", &iAimType, 3);
			ImGui::RadioButton("Smooth Percent##AimType", &iAimType, 4);

			ImGui::Text("X Opt");
			ImGui::RadioButton("None##XOpt", &XType, 0);
			ImGui::RadioButton("Key X##XOpt", &XType, 1);
			ImGui::RadioButton("Shoot X##XOpt", &XType, 2);
		}

		if (ImGui::CollapsingHeader("Other"))
		{
			ImGui::Checkbox("Silent", &Silent);
			ImGui::Checkbox("AutoShot", &bAutoShot);
			ImGui::Checkbox("Capture", &capture);
			if (ImGui::Checkbox("No Spread", &bNoSpread))
			{
				void* code = (void*)0x00740460;

				DWORD OldVP;
				VirtualProtect(code, 3, PAGE_EXECUTE_READWRITE, &OldVP);

				if (bNoSpread)
					memset(code, 0x90, 3);
				else
					memcpy(code, (byte*)"\xD8\x48\x2C", 3);

				VirtualProtect(code, 3, OldVP, &OldVP);
			}
		}

		// 0 = BONE_HEAD
		// 1 = BONE_LEFTHAND
		// 2 = BONE_RIGHTHAND
		// 3 = BONE_LEFTFOOT
		// 4 = BONE_RIGHTFOOT
		// 5 = BONE_SPINE1
		// 6 = BONE_PELVIS

		if (ImGui::CollapsingHeader("Renders"))
		{
			ImGui::Checkbox("FOV Zone", &RendersFOV);
			ImGui::Checkbox("Lines", &RendersLines);
			ImGui::Checkbox("Circle", &RendersCircle);
			ImGui::Checkbox("Bones", &RendersBone);
			ImGui::Checkbox("Boxes", &RendersBoxes);
			ImGui::Checkbox("Bullet Traces", &RendersBT);
			ImGui::Checkbox("Player Names", &RendersPN);
		}

		if (ImGui::CollapsingHeader("Fractions"))
		{
			ImGui::Checkbox("Ignore my team", &bIngnoreT);

			ImGui::RadioButton("None##FractionOpt", &iFractionMode, 0);
			ImGui::RadioButton("Ignore##FractionOpt", &iFractionMode, 1);
			ImGui::RadioButton("Priority##FractionOpt", &iFractionMode, 2);

			ImGui::Checkbox("Cop", &bFractionEnable[0]);
			ImGui::Checkbox("Medic", &bFractionEnable[1]);
			ImGui::Checkbox("Groove", &bFractionEnable[2]);
			ImGui::Checkbox("News", &bFractionEnable[3]);
			ImGui::Checkbox("City Hall", &bFractionEnable[4]);
			ImGui::Checkbox("Rifa", &bFractionEnable[5]);
			ImGui::Checkbox("Army", &bFractionEnable[6]);
			ImGui::Checkbox("La Cosa Nostra", &bFractionEnable[7]);
			ImGui::Checkbox("Vagos", &bFractionEnable[8]);
			ImGui::Checkbox("Russian Mafia", &bFractionEnable[9]);
			ImGui::Checkbox("Aztec", &bFractionEnable[10]);
			ImGui::Checkbox("Ballas", &bFractionEnable[11]);
		}

		if (ImGui::CollapsingHeader("Settings"))
		{
			// WEAPONSLOT_TYPE_HEAVY
			// WEAPONSLOT_TYPE_MG
			// WEAPONSLOT_TYPE_RIFLE
			// WEAPONSLOT_TYPE_SHOTGUN
			// WEAPONSLOT_TYPE_SMG

			ImGui::Text("Universal Slot");
			static char* USlotItems[] = { 
				"GLOBAL", "HEAVY", "MG", "RIFLE", "SHOTGUN", "SMG", "Deagle"
			};
			ImGui::Combo("##USlot", &iCurrentMode, USlotItems, IM_ARRAYSIZE(USlotItems));

			if (iCurrentMode != 0)
				ImGui::Checkbox("Custom", &bEnabled[iCurrentMode - 1]);

			ImGui::Separator();

			ImGui::Text("FOV");
			ImGui::SliderInt("##FOV", &iCurrentFOV[iCurrentMode], 1, 250);
			ImGui::Text("Safe Zone");
			ImGui::SliderFloat("##SafeZone", &fSafeZone[iCurrentMode], 0.001f, 0.025f);
			ImGui::Text("Speed (Only for Fixed)");
			ImGui::SliderInt("##Speed", &iSpeed[iCurrentMode], 1, 30);
			ImGui::Text("Smooth");
			ImGui::SliderFloat("##Smooth", &fSmooth[iCurrentMode], 1.0f, 30.0f);
			ImGui::Text("Smooth Percent");
			ImGui::SliderInt("##SmoothPercent", &iSmoothPercent[iCurrentMode], 1, 99);
			ImGui::Text("Chance % (Only for Silent)");
			ImGui::SliderInt("##ChancePercent", &chance, 1, 100);

			if (XModeBone == 0)
			{
				ImGui::Text("Bone");
				static char* items[] = { "HEAD", "LEFTHAND", "RIGHTHAND", "LEFTFOOT", "RIGHTFOOT", "SPINE", "PELVIS" };
				ImGui::Combo("##Bone", &iBoneId, items, IM_ARRAYSIZE(items));
			}
			ImGui::RadioButton("Fixed Bone##BoneOpt", &XModeBone, 0);
			ImGui::RadioButton("Random Bone##BoneOpt", &XModeBone, 1);
			ImGui::RadioButton("Nearest Bone##BoneOpt", &XModeBone, 2);

			if (ImGui::Button("SAVE"))
				WriteIniInfo();
			if (ImGui::Button("Free"))
				CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)lpFreeLibrary, NULL, NULL, NULL);
			if (ImGui::Button("& Hook"))
				CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)lpAndHook, NULL, NULL, NULL);
		}

		if (ImGui::CollapsingHeader("Info"))
		{
			ImGui::Text("Author: Surse");
			ImGui::Text("Release Build: 5");
			ImGui::Text("Release Date: 24.09.2018");
			if (ImGui::Button("Go to Official Theme"))
				ShellExecute( NULL, "open", "https://blast.hk/threads/24541/",NULL,NULL,SW_SHOW);
		}

		// ImGui_Dark_StyleEditor();
		ImGui::End();

		ImGui::EndFrame();
		// g_D3DDevice->SetRenderState(D3DRS_ZENABLE, false);
		// g_D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		// g_D3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);

		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}

	RendersLoop();

	if (Dark::Key::Check('X') && Dark::Key::CheckJustDown('O') && Dark::SAMP::getInput()->iInputEnabled == 0)
	{
		bMenuState ^= TRUE;
		Dark::SAMP::ToggleCursor(bMenuState);
	}

	if (iTargetPlayer == -1)
	{
		// get target player

		if (bAutoShotDown == true)
			api_KeyEvent(1);

		for (int i = 0; i <= 1004; i++)
		{
			CPed *Ped = getPed(i);
			if (Ped == NULL)
				continue;

			if (Ped->GetHealth() <= 0.0f)
				continue;

			BoneX Bone; 
			if (getBonePosition(Ped, BONE_SPINE1, Bone) == FALSE)
				continue;

			CCamera *pCamera = pGame->GetCamera();
			CCamSAInterface *pCam = (CCamSAInterface *)((CCamSA *)pCamera->GetCam(pCamera->GetActiveCam()))->GetInterface();
			CVector src = pCam->Source;
			if (Dark::GAME::getGameInterface()->GetWorld()->IsLineOfSightClear(&src, &Bone.x3D, true, false, false, true, true, false, false) == FALSE)
				continue;

			if (!Dark::Key::Check(VK_RBUTTON))
				continue;

			POINT2D Crosshair = GetCrosshairPosition();

			if (Crosshair.x < Bone.x2D.fX + (iCurrentFOV[iCurrentModeEV] / 2) &&
				Crosshair.x > Bone.x2D.fX - (iCurrentFOV[iCurrentModeEV] / 2) &&
				Crosshair.y < Bone.x2D.fY + (iCurrentFOV[iCurrentModeEV] / 2) &&
				Crosshair.y > Bone.x2D.fY - (iCurrentFOV[iCurrentModeEV] / 2))
			{
				if (XModeBone == 0)
					XBone = iBoneId;
				if (XModeBone == 1)
					XBone = FindNearestBone(Ped);
				if (XModeBone == 2)
					XBone = rand() % 6;

				iTargetPlayer = i;
				break;
			}
		}
	}
	else
	{
		if (Dark::GAME::getActorInfo(ACTOR_SELF, ACTOR_ALIVE) == NULL || !Dark::GAME::getPlayerPed()->IsOnScreen())
		{
			iTargetPlayer = -1;
			return;
		}

		if (!Dark::Key::Check(VK_RBUTTON))
		{
			iTargetPlayer = -1;
			return;
		}

		CPed *Ped = getPed(iTargetPlayer);
		if (Ped == NULL)
		{
			iTargetPlayer = -1;
			return;
		}

		if (Ped->GetHealth() <= 0.0f)
		{
			iTargetPlayer = -1;
			return;
		}
		
		BoneX Bone;
		if (getBonePosition(Ped, BONE_SPINE1, Bone) == FALSE)
		{
			iTargetPlayer = -1;
			return;
		}

		CCamera *pCamera = pGame->GetCamera();
		CCamSAInterface *pCam = (CCamSAInterface *)((CCamSA *)pCamera->GetCam(pCamera->GetActiveCam()))->GetInterface();
		CVector src = pCam->Source;
		if (Dark::GAME::getGameInterface()->GetWorld()->IsLineOfSightClear(&src, &Bone.x3D, true, false, false, true, true, false, false) == FALSE)
		{
			iTargetPlayer = -1;
			return;
		}

		POINT2D Crosshair = GetCrosshairPosition();

		bool next = (Crosshair.x < Bone.x2D.fX + (iCurrentFOV[iCurrentModeEV] / 2) &&
			Crosshair.x > Bone.x2D.fX - (iCurrentFOV[iCurrentModeEV] / 2) &&
			Crosshair.y < Bone.x2D.fY + (iCurrentFOV[iCurrentModeEV] / 2) &&
			Crosshair.y > Bone.x2D.fY - (iCurrentFOV[iCurrentModeEV] / 2));
		if (capture) next = true;

		if (next)
		{
			eWeaponSlot selfSlot = Dark::GAME::getPlayerPed()->GetCurrentWeaponSlot();
			switch (selfSlot)
			{
			case WEAPONSLOT_TYPE_UNARMED:
			case WEAPONSLOT_TYPE_MELEE:
			case WEAPONSLOT_TYPE_THROWN:
			case WEAPONSLOT_TYPE_SPECIAL:
			case WEAPONSLOT_TYPE_GIFT:
			case WEAPONSLOT_TYPE_PARACHUTE:
			case WEAPONSLOT_TYPE_DETONATOR:
				// we don't want to aim for these weapons
				return;
			}

			DWORD color = Dark::SAMP::getPlayerColor(iTargetPlayer);

			if (iFractionMode != 0)
			{
				bool bSearched = false;
				for (int c = 0; c < 12; c++)
				{
					if (color == dwPlayerColors[c])
						bSearched = true;
				}

				if (iFractionMode == 1 && bSearched)
					return;
				if (iFractionMode == 2 && !bSearched)
					return;
			}

			if (bIngnoreT)
			{
				if (color == Dark::SAMP::getPlayerColor(Dark::SAMP::getPlayerPool()->sLocalPlayerID))
					return;
			}

			eBone pGtaBone;

			switch (XBone)
			{
			case 0:
			{
				pGtaBone = BONE_HEAD;
				break;
			}
			case 1:
			{
				pGtaBone = BONE_LEFTHAND;
				break;
			}
			case 2:
			{
				pGtaBone = BONE_RIGHTHAND;
				break;
			}
			case 3:
			{
				pGtaBone = BONE_LEFTFOOT;
				break;
			}
			case 4:
			{
				pGtaBone = BONE_RIGHTFOOT;
				break;
			}
			case 5:
			{
				pGtaBone = BONE_SPINE1;
				break;
			}
			case 6:
			{
				pGtaBone = BONE_PELVIS;
				break;
			}
			default:
				break;
			}

			bool bEvent = false;
			if (XType == 0)
				bEvent = true;
			if (XType == 1)
			{
				if (Dark::Key::Check(VK_LBUTTON))
					bEvent = true;
			}
			if (XType == 2)
			{
				if (XOnShoot)
					bEvent = true;
			}

			if (bEvent)
			{
				if (iAimType == 1)
				{
					CamOffset Offset = getCamOffset(Ped, pGtaBone);
					pCam->Alpha = Offset.Alpha;
					pCam->Beta = Offset.Beta;
				}
				else
				{
					CamOffset Offset = getCamOffset(Ped, pGtaBone);
					pCam->Alpha = cheat_AimEvent(pCam->Alpha, Offset.Alpha);
					if (Offset.Beta > M_PI) Offset.Beta = -((M_PI * 2) - Offset.Beta);
					pCam->Beta = cheat_AimEvent(pCam->Beta, Offset.Beta);
				}
			}

			if (bAutoShot)
			{
				BoneX X22; getBonePosition(Ped, (eBone)22, X22); // Left
				BoneX X32; getBonePosition(Ped, (eBone)32, X32); // Right
				BoneX X1; getBonePosition(Ped, (eBone)1, X1); // Bottom
				BoneX X4; getBonePosition(Ped, (eBone)4, X4); // Top

				float left, right = 0;
				if (X22.x2D.fX > X32.x2D.fX)
				{
					left = X32.x2D.fX;
					right = X22.x2D.fX;
				}
				else
				{
					left = X22.x2D.fX;
					right = X32.x2D.fX;
				}

				if (Crosshair.x > left &&
					Crosshair.x < right &&
					Crosshair.y > X4.x2D.fY &&
					Crosshair.y < X1.x2D.fY)
				{
					int weapon = Dark::SAMP::getPlayerPool()->pLocalPlayer->byteCurrentWeapon;

					if (weapon == 22 ||
						weapon == 23 ||
						weapon == 24 ||
						weapon == 25 ||
						weapon == 26 ||
						weapon == 27 ||
						weapon == 33)
						api_KeyEvent(2);
					else
						api_KeyEvent(0);
				}
				else if (bAutoShotDown == true)
					api_KeyEvent(1);
			}
			else if (bAutoShotDown == true)
				api_KeyEvent(1);
		}
		else
		{
			if (bAutoShotDown == true)
				api_KeyEvent(1);
			iTargetPlayer = -1;
			return;
		}
	}

	if (XOnShoot)
		XOnShoot = false;
}
