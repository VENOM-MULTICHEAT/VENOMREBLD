#include "plugin.h"

IDirect3DStateBlock9		*pStateBlock = NULL;
IDirect3DStateBlock9		*pStateBlockDraw = NULL;
static const int			CIRCLE_RESOLUTION = 64;

IDirect3DDevice9 *DrawDevice9 = NULL;

IDirect3DDevice9 *Dark::Render::DrawDevice()
{
	return DrawDevice9;
}

IDirect3DDevice9 *Dark::Render::DrawDevice(IDirect3DDevice9 *dev)
{
	DrawDevice9 = dev;
	return DrawDevice9;
}

float PID = 1.0f;

void Dark::Render::Initialise()
{
	for (int i = 0; i <= 1; i++)
	{
		Dark::Render::DrawDevice()->BeginStateBlock();
		Dark::Render::DrawDevice()->SetPixelShader(NULL);
		Dark::Render::DrawDevice()->SetVertexShader(NULL);

		Dark::Render::DrawDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		Dark::Render::DrawDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		Dark::Render::DrawDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		Dark::Render::DrawDevice()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		Dark::Render::DrawDevice()->SetRenderState(D3DRS_ALPHAREF, 0x08);
		Dark::Render::DrawDevice()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		Dark::Render::DrawDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		Dark::Render::DrawDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		Dark::Render::DrawDevice()->SetRenderState(D3DRS_STENCILENABLE, FALSE);
		Dark::Render::DrawDevice()->SetRenderState(D3DRS_CLIPPING, TRUE);
		Dark::Render::DrawDevice()->SetRenderState(D3DRS_CLIPPLANEENABLE, FALSE);
		Dark::Render::DrawDevice()->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
		Dark::Render::DrawDevice()->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
		Dark::Render::DrawDevice()->SetRenderState(D3DRS_FOGENABLE, FALSE);
		Dark::Render::DrawDevice()->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
		Dark::Render::DrawDevice()->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |
			D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);

		Dark::Render::DrawDevice()->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		Dark::Render::DrawDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		Dark::Render::DrawDevice()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		Dark::Render::DrawDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		Dark::Render::DrawDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		Dark::Render::DrawDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
		Dark::Render::DrawDevice()->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
		Dark::Render::DrawDevice()->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
		Dark::Render::DrawDevice()->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		Dark::Render::DrawDevice()->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		Dark::Render::DrawDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		Dark::Render::DrawDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		Dark::Render::DrawDevice()->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

		if (i == 0) Dark::Render::DrawDevice()->EndStateBlock(&pStateBlock);
		else		Dark::Render::DrawDevice()->EndStateBlock(&pStateBlockDraw);
	}

	return;
}

void Dark::Render::Release()
{
	SAFE_RELEASE(pStateBlock);
	SAFE_RELEASE(pStateBlockDraw);
}

void Dark::Render::ResetFont(LPD3DXFONT &pFont, bool dev)
{
	if (!pFont) return;

	if (dev)	pFont->OnResetDevice();
	else		pFont->OnLostDevice();
}

void BeginRender()
{
	if (pStateBlock == NULL || pStateBlockDraw == NULL)
	{
		Dark::Render::Release();
		Dark::Render::Initialise();
	}

	pStateBlock->Capture();
	pStateBlockDraw->Apply();
}

void EndRender()
{
	pStateBlock->Apply();
}

////////////////////////////////////////////////////////////////////////////////

HRESULT Dark::Render::LoadFont(LPD3DXFONT &pFont, char* fontname, int height, int weight)
{
	if (pFont) pFont->Release();

	D3DXFONT_DESC m_fontdesc;
	ZeroMemory(&m_fontdesc, sizeof(D3DXFONT_DESC));
	m_fontdesc.Height = height;
	m_fontdesc.Width = 7;
	m_fontdesc.Weight = weight;
	m_fontdesc.MipLevels = D3DX_DEFAULT;
	m_fontdesc.Italic = false;
	m_fontdesc.CharSet = DEFAULT_CHARSET;
	m_fontdesc.OutputPrecision = OUT_DEFAULT_PRECIS;
	m_fontdesc.Quality = ANTIALIASED_QUALITY;
	m_fontdesc.PitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	strcpy(m_fontdesc.FaceName, fontname);

	if (FAILED(D3DXCreateFontIndirect(Dark::Render::DrawDevice(), &m_fontdesc, &pFont)))
	{
		Dark::Log("Error: D3DXCreateFontIndirect()");
		return E_FAIL;
	}

	return D3D_OK;
}

void Dark::Render::Print(LPD3DXFONT &pFont, char *text, int x, int y, int w, int h, bool border, D3DCOLOR color, DWORD format)
{
	if (!pFont) return;

	RECT rect;

	switch (format)
	{

	case DT_TOP:
	{
		rect.top = y;
		rect.bottom = y + h;
		break;
	}

	case DT_BOTTOM:
	{
		rect.bottom = y;
		rect.top = y - h;
		break;
	}

	default:
		break;

	}

	rect.left = x;
	rect.right = x + w;

	if (border)
	{
		RECT bRect = { rect.left, rect.top - 1, rect.right, rect.bottom - 1 };
		pFont->DrawText(NULL, text, -1, &bRect, format, D3DCOLOR_XRGB(0, 0, 0));

		bRect = { rect.left - 1, rect.top, rect.right - 1, rect.bottom };
		pFont->DrawText(NULL, text, -1, &bRect, format, D3DCOLOR_XRGB(0, 0, 0));

		bRect = { rect.left + 1, rect.top, rect.right + 1, rect.bottom };
		pFont->DrawText(NULL, text, -1, &bRect, format, D3DCOLOR_XRGB(0, 0, 0));

		bRect = { rect.left, rect.top + 1, rect.right, rect.bottom + 1 };
		pFont->DrawText(NULL, text, -1, &bRect, format, D3DCOLOR_XRGB(0, 0, 0));
	}

	pFont->DrawText(NULL, text, -1, &rect, format, color);
}

void Dark::Render::Print(LPD3DXFONT &pFont, char *text, int x, int y, bool border, D3DCOLOR color)
{
	if (!pFont) return;

	int w = Dark::Render::FontWidth(pFont, text);
	int h = Dark::Render::FontHeight(pFont);

	RECT rect = { x, y, x + w, y + h };

	if (border)
	{
		RECT bRect = { rect.left, rect.top - 1, rect.right, rect.bottom - 1 };
		pFont->DrawText(NULL, text, -1, &bRect, DT_TOP | DT_LEFT, D3DCOLOR_XRGB(0, 0, 0));

		bRect = { rect.left - 1, rect.top, rect.right - 1, rect.bottom };
		pFont->DrawText(NULL, text, -1, &bRect, DT_TOP | DT_LEFT, D3DCOLOR_XRGB(0, 0, 0));

		bRect = { rect.left + 1, rect.top, rect.right + 1, rect.bottom };
		pFont->DrawText(NULL, text, -1, &bRect, DT_TOP | DT_LEFT, D3DCOLOR_XRGB(0, 0, 0));

		bRect = { rect.left, rect.top + 1, rect.right, rect.bottom + 1 };
		pFont->DrawText(NULL, text, -1, &bRect, DT_TOP | DT_LEFT, D3DCOLOR_XRGB(0, 0, 0));
	}

	pFont->DrawText(NULL, text, -1, &rect, DT_TOP | DT_LEFT, color);
}

int Dark::Render::FontWidth(LPD3DXFONT &pFont, char *text)
{
	if (!pFont) return 0;

	RECT rect = { 0,0,0,0 };
	pFont->DrawText(NULL, text, strlen(text), &rect, DT_CALCRECT, D3DCOLOR_XRGB(0, 0, 0));
	return rect.right - rect.left;
}

int Dark::Render::FontHeight(LPD3DXFONT &pFont)
{
	if (!pFont) return 0;

	D3DXFONT_DESC m_fontdesc;
	pFont->GetDesc(&m_fontdesc);

	return m_fontdesc.Height;
}

POINT2D Dark::Render::CursorPos()
{
	POINT pos;

	if (GetCursorPos(&pos) && ScreenToClient(Dark::Render::getWindow(), &pos))
		return { pos.x, pos.y };
	return { 0, 0 };
}

////////////////////////////////////////////////////////////////////////////////

void Dark::Render::DrawTriangle(POINT2D one, POINT2D two, POINT2D three, D3DCOLOR color)
{
	BeginRender();

	D3DVERTEX vertex[3] = {
		{ (float)one.x, (float)one.y, 0, 1.0f, color },
		{ (float)two.x, (float)two.y, 0, 1.0f, color },
		{ (float)three.x, (float)three.y, 0, 1.0f, color }
	};

	IDirect3DBaseTexture9* pTexture = NULL;
	DWORD dwFVF;

	Dark::Render::DrawDevice()->GetTexture(0, &pTexture);
	Dark::Render::DrawDevice()->GetFVF(&dwFVF);

	Dark::Render::DrawDevice()->SetTexture(0, NULL);
	Dark::Render::DrawDevice()->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	Dark::Render::DrawDevice()->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, vertex, sizeof(D3DVERTEX));

	Dark::Render::DrawDevice()->SetTexture(0, pTexture);
	Dark::Render::DrawDevice()->SetFVF(dwFVF);

	EndRender();
}

void Dark::Render::DrawBox(int x, int y, int w, int h, D3DCOLOR color)
{
	BeginRender();

	D3DVERTEX vertex[4] = {
		{ (float)x, (float)y, 0, 1.0f, color },
		{ (float)x + (float)w, (float)y, 0, 1.0f, color },
		{ (float)x, (float)y + (float)h, 0, 1.0f, color },
		{ (float)x + (float)w, (float)y + (float)h, 0, 1.0f, color } };

	IDirect3DBaseTexture9* pTexture = NULL;
	DWORD dwFVF;

	Dark::Render::DrawDevice()->GetTexture(0, &pTexture);
	Dark::Render::DrawDevice()->GetFVF(&dwFVF);

	Dark::Render::DrawDevice()->SetTexture(0, NULL);
	Dark::Render::DrawDevice()->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	Dark::Render::DrawDevice()->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(D3DVERTEX));

	Dark::Render::DrawDevice()->SetTexture(0, pTexture);
	Dark::Render::DrawDevice()->SetFVF(dwFVF);

	EndRender();
}

void Dark::Render::DrawBorderBox(int x, int y, int w, int h, int border, D3DCOLOR color, D3DCOLOR bordercolor)
{
	Dark::Render::DrawBox(x, y, w, h, bordercolor);
	Dark::Render::DrawBox(x + border, y + border, w - border * 2, h - border * 2, color);
}

void Dark::Render::DrawLine(int x1, int y1, int x2, int y2, D3DCOLOR color)
{
	BeginRender();

	D3DVERTEX vertex[2] = {
		{ (float)x1, (float)y1, 0, 1.0f, color },
		{ (float)x2, (float)y2, 0, 1.0f, color }
	};

	IDirect3DBaseTexture9* pTexture = NULL;
	DWORD dwFVF;

	Dark::Render::DrawDevice()->GetTexture(0, &pTexture);
	Dark::Render::DrawDevice()->GetFVF(&dwFVF);

	Dark::Render::DrawDevice()->SetTexture(0, NULL);
	Dark::Render::DrawDevice()->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	Dark::Render::DrawDevice()->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, vertex, sizeof(D3DVERTEX));

	Dark::Render::DrawDevice()->SetTexture(0, pTexture);
	Dark::Render::DrawDevice()->SetFVF(dwFVF);

	EndRender();
}

void Dark::Render::DrawCircle(int x, int y, int radius, bool filled, D3DCOLOR color)
{
	BeginRender();

	const int points = 64;

	D3DVERTEX* pVertex = new D3DVERTEX[points + 1];
	D3DPRIMITIVETYPE type;

	if (filled)
	{
		for (int i = 0; i <= points; i++) pVertex[i] = { x + radius * cos(D3DX_PI * (i / (points / 2.0f))), y + radius * sin(D3DX_PI * (i / (points / 2.0f))), 0.0f, 1.0f, color };
		type = D3DPT_TRIANGLEFAN;
	}
	else
	{
		for (int i = 0; i <= points; i++) pVertex[i] = { x + radius * cos(D3DX_PI * (i / (points / 2.0f))), y - radius * sin(D3DX_PI * (i / (points / 2.0f))), 0.0f, 1.0f, color };
		type = D3DPT_LINESTRIP;
	}
	
	IDirect3DBaseTexture9* pTexture = NULL;
	DWORD dwFVF;

	Dark::Render::DrawDevice()->GetTexture(0, &pTexture);
	Dark::Render::DrawDevice()->GetFVF(&dwFVF);

	Dark::Render::DrawDevice()->SetTexture(0, NULL);
	Dark::Render::DrawDevice()->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	Dark::Render::DrawDevice()->DrawPrimitiveUP(type, points, pVertex, sizeof(D3DVERTEX));
	delete[] pVertex;

	Dark::Render::DrawDevice()->SetTexture(0, pTexture);
	Dark::Render::DrawDevice()->SetFVF(dwFVF);

	EndRender();
}
