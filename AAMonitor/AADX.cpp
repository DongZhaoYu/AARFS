/*
*	@file: AADX.cpp
*	@brief: the implementation of the basic tool for drawing a shape
*	@author: dongzhaoyu
*	@date: 2008-4-15
*
*	Copyright (c) 2008 CTEC Xi'an JiaoTong University
*/

#include "AADX.h"
#include <assert.h>
#include <cmath>
#include <D3DX10Math.h>

//--------------------------------------------------------------------------
//the implementation of AADX
//--------------------------------------------------------------------------
#pragma warning (disable : 4244)
#pragma warning (push)
#pragma warning (disable : 4005)
#include <tchar.h>
#pragma warning (pop)

AADX::AADX() : m_hWnd(NULL),
			   m_pD3D(NULL),
			   m_pD3DDevice(NULL),
			   m_bInited(FALSE),
			   m_bBegin(FALSE),
			   m_pSprite(NULL),
			   m_pLine(NULL)
{}

AADX::AADX(HWND hWnd) : m_hWnd(hWnd),
			   m_pD3D(NULL),
			   m_pD3DDevice(NULL),
			   m_bInited(FALSE),
			   m_bBegin(FALSE),
			   m_pSprite(NULL),
			   m_pLine(NULL)
{}

AADX::~AADX()
{
	Cleanup();
}

Bool AADX::Init3D(HWND hWnd, Bool fullscreen)
{
	if (m_bInited)
		return TRUE;

	HWND hwnd = (hWnd == NULL) ? m_hWnd : hWnd;
	if (hwnd == NULL)
		return FALSE;

	if ((m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return FALSE;
	
	D3DCAPS9 d3dcaps;
	m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dcaps);
	Bool hardwareprocess = FALSE;
	if (d3dcaps.DevCaps & D3DDEVCAPS_PUREDEVICE)
	{
		hardwareprocess = TRUE;
	}
	D3DDISPLAYMODE d3ddisplaymode;
	if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddisplaymode)))
		return FALSE;

	RECT rc;
	GetClientRect(m_hWnd, &rc);
	m_WWidth = rc.right - rc.left;
	m_WHeight = rc.bottom - rc.top;

    D3DPRESENT_PARAMETERS d3dpara;
	ZeroMemory(&d3dpara, sizeof(d3dpara));
	d3dpara.Windowed = !fullscreen;
	d3dpara.hDeviceWindow = m_hWnd;
	d3dpara.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	d3dpara.SwapEffect = D3DSWAPEFFECT_FLIP;
	d3dpara.BackBufferFormat = d3ddisplaymode.Format;
	d3dpara.BackBufferCount = 2;
	d3dpara.BackBufferHeight = m_WHeight;
	d3dpara.BackBufferWidth = m_WWidth;

	if (!hardwareprocess && FAILED(this->m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
										D3DDEVTYPE_HAL,
										hwnd,
										D3DCREATE_SOFTWARE_VERTEXPROCESSING,
										&d3dpara,
										&m_pD3DDevice)))
		return FALSE;
	if (hardwareprocess && FAILED(this->m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
									D3DDEVTYPE_HAL,
									hwnd,
									D3DCREATE_HARDWARE_VERTEXPROCESSING,
									&d3dpara,
									&m_pD3DDevice)))
		return FALSE;
	if (FAILED(D3DXCreateSprite(m_pD3DDevice, &m_pSprite)))
		return FALSE;
	if (FAILED(D3DXCreateLine(m_pD3DDevice, &m_pLine)))
		return FALSE;

	//Declare this process to be high DPI aware, and prevent automatic scaling 
    HINSTANCE hUser32 = LoadLibrary( L"user32.dll" );
    if( hUser32 )
    {
        typedef BOOL (WINAPI * LPSetProcessDPIAware)(void);
        LPSetProcessDPIAware pSetProcessDPIAware = (LPSetProcessDPIAware)GetProcAddress( hUser32, "SetProcessDPIAware" );
        if( pSetProcessDPIAware )
        {
            pSetProcessDPIAware();
        }
        FreeLibrary( hUser32 );
    }

	m_hWnd = hwnd;
	m_bInited = TRUE;
	return TRUE;
}

void AADX::BeginRender(U8 r, U8 g, U8 b)
{
	assert(m_bInited == TRUE);

	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(r, g, b), 0.0f, 0);
	if (SUCCEEDED(m_pD3DDevice->BeginScene()))
	{
		m_bBegin = TRUE;
	}
}

void AADX::EndRender()
{
	assert(m_bBegin == TRUE);

	m_pD3DDevice->EndScene();
	m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
	m_bBegin = FALSE;
}

Bool AADX::SetVertex(const RECT &rc, U32 color, AAPVERTEXBUFFER &buffer, U32 &count) const
{
	CUSTOMVERTEX customvertex[] = 
	{
		{rc.left, rc.top, 0.0f, 1.0f, color},
		{rc.right, rc.top, 0.0f, 1.0f, color},
		{rc.right, rc.bottom, 0.0f, 1.0f, color},
		{rc.left, rc.bottom, 0.0f,1.0f, color},
		{rc.left, rc.top, 0.0f, 1.0f, color}
	};

	if (FAILED(m_pD3DDevice->CreateVertexBuffer(sizeof(customvertex),
								0,
								AAD3DCUSTOMVERTEX_FVF,
								D3DPOOL_MANAGED,
								&buffer,
								NULL)))
	{
		return FALSE;
	}
	
	void *pvertice;
	if (FAILED(buffer->Lock(0, sizeof(customvertex), (void**)&pvertice, 0)))
	{
		return FALSE;
	}

	memcpy(pvertice, customvertex, sizeof(customvertex));
	buffer->Unlock();
	count = 4;
	return TRUE;
}

Bool AADX::SetVertex(const POINT &p, U32 r, U32 color, AAPVERTEXBUFFER &buffer, U32 &ncount) const //we now do not use this method
{
#define sita 0.2f
#define dpi 3.1415f * 2

	CUSTOMVERTEX vertice[(int)(dpi/sita)+3];	//3 = 1+1+1 to be safe
	U32 count = 0;
	for (F32 alpha = 0; alpha <= dpi; alpha += sita)
	{
		vertice[count].x = r * cos(alpha) + p.x;
		vertice[count].y = r * sin(alpha) + p.y;
		vertice[count].z = 0.0f;
		vertice[count].rhw = 1.0f;
		vertice[count].color = color;
		count++;
	}
	vertice[count].x = r + p.x;
	vertice[count].y = p.y;
	vertice[count].z = 0.0f;
	vertice[count].rhw = 1.0f;
	vertice[count].color = color;
	count++;

	if (FAILED(m_pD3DDevice->CreateVertexBuffer(sizeof(CUSTOMVERTEX) * count,
												0,
												AAD3DCUSTOMVERTEX_FVF,
												D3DPOOL_MANAGED,
												&buffer,
												NULL)))
		return FALSE;

	void *data;
	if (FAILED(buffer->Lock(0, sizeof(CUSTOMVERTEX) * count, (void**)&data, 0)))
		return FALSE;

	memcpy(data, vertice, count * sizeof(CUSTOMVERTEX));
	buffer->Unlock();
	ncount = count - 1;
	return TRUE;
}

Bool AADX::SetBuffer(U32 size, AAPVERTEXBUFFER &buffer) const
{
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(size, 
												0,
												AAD3DCUSTOMVERTEX_FVF,
												D3DPOOL_MANAGED,
												&buffer,
												NULL)))
		return FALSE;
	return TRUE;
}

Bool AADX::SetVertex(CUSTOMVERTEX *vertex, U32 vertexlen, AAPVERTEXBUFFER buffer)
{ return TRUE;}

Bool AADX::SetVertexB(const POINT &p, U32 r, U32 mcolor, AAPVERTEXBUFFER &buffer, U32 &count) const //bresenham algorithm
{
#define MH(x, y, d) { (x) = (x) + 1; (d) = (d) + ((x) << 1) + 1;}
#define MD(x, y, d) { (x) = (x) + 1; (y) = (y) - 1; (d) = (d) + ((x) << 1) - ((y) << 1) + 2;}
#define MV(x, y, d) { (y) = (y) - 1; (d) = (d) - ((y) << 1) + 1;}

	S32 pixelnum = 0;
	S32 limit = 0;
	CUSTOMVERTEX *pvertice = new CUSTOMVERTEX[r * 7];
	S32 cy = r, cx = 0, delta = (1 - r) << 1, phy = 0;

	while (cy >= limit)
	{
		pvertice[pixelnum].x = cx;
		pvertice[pixelnum].y = cy;
		pvertice[pixelnum].z = 0.0f;
		pvertice[pixelnum].color = mcolor;
		pvertice[pixelnum].rhw = 1.0f;
		pixelnum++;

		if (delta < 0)
		{
			phy = ((delta + cy) << 1) - 1;
			if (phy <= 0)
				MH(cx, cy, delta)
			else
				MD(cx, cy, delta)
		}
		else if (delta > 0)
		{
			phy = ((delta - cx) << 1) - 1;
			if (phy <= 0)
				MD(cx, cy, delta)
			else
				MV(cx, cy, delta)
		}
		else
			MD(cx, cy, delta)
	}

	S32 current = 0;
	for (current = pixelnum - 1; current >= 0; current--, pixelnum++)
	{
		pvertice[pixelnum].x = pvertice[current].x;
		pvertice[pixelnum].y = -pvertice[current].y;
		pvertice[pixelnum].z = pvertice[current].z;
		pvertice[pixelnum].color = pvertice[current].color;
		pvertice[pixelnum].rhw = pvertice[current].rhw;
	}

	for (current = pixelnum - 1; current >= 0; current--, pixelnum++)
	{
		pvertice[pixelnum].x = -pvertice[current].x;
		pvertice[pixelnum].x += p.x;
		pvertice[current].x += p.x;

		pvertice[current].y += p.y;
		pvertice[pixelnum].y = pvertice[current].y;

		pvertice[pixelnum].z = pvertice[current].z;
		pvertice[pixelnum].rhw = pvertice[current].rhw;
		pvertice[pixelnum].color = pvertice[current].color;
	}
	
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(sizeof(CUSTOMVERTEX) * pixelnum,
												NULL,
												AAD3DCUSTOMVERTEX_FVF,
												D3DPOOL_MANAGED,
												&buffer,
												NULL)))
		return FALSE;
	void *data = NULL;
	if (FAILED(buffer->Lock(0, sizeof(CUSTOMVERTEX) * pixelnum, (void**)&data, NULL)))
		return FALSE;
	memcpy(data, pvertice, sizeof(CUSTOMVERTEX) * pixelnum);
	buffer->Unlock();
	count = pixelnum;
	delete[] pvertice;
	return TRUE;
}

Bool AADX::SetVertex(const POINT &start, const POINT &end, U32 color, AAPVERTEXBUFFER &buffer) const
{
	CUSTOMVERTEX vertice[2] = {
		{start.x, start.y, 0.0f, 1.0f, color},
		{end.x, end.y, 0.0f, 1.0f, color}
	};
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(sizeof(vertice), NULL, AAD3DCUSTOMVERTEX_FVF, D3DPOOL_MANAGED, &buffer, NULL)))
		return FALSE;
	void *data = NULL;
	if (FAILED(buffer->Lock(0, sizeof(vertice), (void**)&data, NULL)))
		return FALSE;
	memcpy(data, vertice, sizeof(vertice));
	buffer->Unlock();
	return TRUE;
}

void AADX::DrawVertex(const AAPVERTEXBUFFER buffer, const U32 count, const DRAWSTYLE style) const
{
	m_pD3DDevice->SetStreamSource(0, buffer, 0, sizeof(CUSTOMVERTEX));
	m_pD3DDevice->SetFVF(AAD3DCUSTOMVERTEX_FVF);
	if (style == POINTLIST)
		m_pD3DDevice->DrawPrimitive(D3DPT_POINTLIST, 0, count);
	else if (style == LINELIST)
		m_pD3DDevice->DrawPrimitive(D3DPT_LINELIST, 0, count);
	else if (style == POLYLINE)
		m_pD3DDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, count);
	else if (style == TRIANGLELIST)
		m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, count);
	else
		m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, count);
}

Bool AADX::CreateSprite(LPD3DXSPRITE &sprite) const
{
	if (m_pD3DDevice == NULL || m_bInited == FALSE || m_pSprite == NULL)
		return FALSE;
	sprite = m_pSprite;
	return TRUE;
}

Bool AADX::CreateAATexture(U32 sourceid, LPDIRECT3DTEXTURE9 &texture) const
{
	HMODULE hModule = GetModuleHandle(_T("AARFS.exe"));
	HGLOBAL hRes = FindResource(hModule, MAKEINTRESOURCE(sourceid), _T("BITMAP"));
	if (FAILED(D3DXCreateTextureFromResource(m_pD3DDevice, hModule, MAKEINTRESOURCE(sourceid), &texture)))
		return FALSE;
	return TRUE;
}

Bool AADX::CreateAATexture(LPCTSTR filename, LPDIRECT3DTEXTURE9 &texture, U32 &width, U32 &height) const
{
	D3DXIMAGE_INFO info;
	ZeroMemory(&info, sizeof(info));
	if (FAILED(D3DXCreateTextureFromFileEx(m_pD3DDevice, 
											filename, 
											D3DX_DEFAULT,
											D3DX_DEFAULT,
											1,
											0,
											D3DFMT_A8R8G8B8,
											D3DPOOL_MANAGED,
											D3DX_DEFAULT,
											D3DX_DEFAULT,
											0,
											&info,
											NULL,
											&texture)))
		return FALSE;
	width = info.Width;
	height = info.Height;
	return TRUE;
}

Bool AADX::CreateAAFont(const D3DXFONT_DESC &fontdesc, LPD3DXFONT &font) const
{
	if (FAILED(D3DXCreateFontIndirect(m_pD3DDevice, &fontdesc, &font)))
		return FALSE;
	return TRUE;
}

void AADX::CreateLine(LPD3DXLINE &line)
{
	if (m_pLine == NULL)
		D3DXCreateLine(m_pD3DDevice, &m_pLine);
	line = m_pLine;
}

void AADX::Cleanup()
{
	if (this->m_pD3DDevice != NULL)
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}
	if (this->m_pD3D != NULL)
	{
		m_pD3D->Release();
		m_pD3D = NULL;
	}
	if (this->m_pSprite != NULL)
	{
		m_pSprite->Release();
		m_pSprite = NULL;
	}
	if (m_pLine != NULL)
	{
		m_pLine->Release();
		m_pLine = NULL;
	}
}

//----------------------------------------------------------------------------------------------------------
//the implementation of AALineImpl
//----------------------------------------------------------------------------------------------------------
AALineImpl::AALineImpl() : m_pVertice(NULL), m_pAADX(NULL)
{}

AALineImpl::AALineImpl(const POINT &start, const POINT &end, U32 color, PAADX aadx) : m_pAADX(aadx), 
																					m_pVertice(NULL),
																					m_Color(color)
{
	m_Start = start;
	m_End = end;
}

AALineImpl::~AALineImpl()
{
	if (m_pVertice != NULL)
		m_pVertice->Release();
	m_pVertice = NULL;
	m_pAADX = NULL;
}

void AALineImpl::Draw()
{
	if (m_pAADX == NULL)
		return;
	if (m_pVertice == NULL)
		m_pAADX->SetVertex(m_Start, m_End, m_Color, m_pVertice);
	m_pAADX->DrawVertex(m_pVertice, 1);
}
//----------------------------------------------------------------------------------------------------------
//the implementation of AARectImpl
//----------------------------------------------------------------------------------------------------------
AARectImpl::AARectImpl(const PAADX aadx) : m_pAADX(aadx), 
										m_Color(0xff000000),
										m_pVertice(NULL),
										m_VertexCount(0)
{
	this->m_RC.left = 0;
	this->m_RC.right = 1;
	this->m_RC.top = 0;
	this->m_RC.bottom = 1;
}

AARectImpl::AARectImpl(const PAADX aadx, U32 color, const RECT &rc) : m_pAADX(aadx), 
																	m_Color(color),
																	m_pVertice(NULL),
																	m_VertexCount(0)
{
	this->m_RC.left = rc.left;
	this->m_RC.right = rc.right;
	this->m_RC.top = rc.top;
	this->m_RC.bottom = rc.bottom;
}

AARectImpl::~AARectImpl()
{
	if (m_pVertice != NULL)
		m_pVertice->Release();
	m_pVertice = NULL;
	m_pAADX = NULL;
}

Bool AARectImpl::SetVertex()
{
	if (m_pAADX == NULL || m_pAADX->GetbInited() == FALSE)
		return FALSE;
	m_pAADX->SetVertex(this->m_RC, this->m_Color, this->m_pVertice, this->m_VertexCount);
	return TRUE;
}

Bool AARectImpl::IsIn(const POINT &point) const
{
	return (m_RC.left <= point.x && m_RC.right >= point.x && m_RC.top <= point.y && m_RC.bottom >= point.y);
}

void AARectImpl::Draw()
{
	if (m_pVertice == NULL && SetVertex()) NULL;
	if (m_pAADX != NULL)
		m_pAADX->DrawVertex(m_pVertice, m_VertexCount, POLYLINE);
}

void AARectImpl::GetPos(RECT &rc) const
{
	rc.left = m_RC.left;
	rc.right = m_RC.right;
	rc.top = m_RC.top;
	rc.bottom = m_RC.bottom;
}

//-------------------------------------------------------------------------------------------------------------------
//the implementation if AACircleImpl
//-------------------------------------------------------------------------------------------------------------------
AACircleImpl::AACircleImpl() : m_pVertice(NULL), m_pAADX(NULL), m_Radius(0), m_VertexCount(0), m_Color(0x00ff000000)
{
	ZeroMemory(&m_Center, sizeof(m_Center));
}

AACircleImpl::AACircleImpl(const POINT &p, U32 radius, U32 color, PAADX aadx) : m_pAADX(aadx),
																				m_Color(color),
																				m_Radius(radius),
																				m_VertexCount(0),
																				m_pVertice(NULL)
{
	m_Center.x = p.x;
	m_Center.y = p.y;
}

AACircleImpl::~AACircleImpl()
{
	m_pAADX = NULL;
	if (m_pVertice != NULL)
		m_pVertice->Release();
	m_pVertice = NULL;
}

Bool AACircleImpl::SetVertex()
{
	if (m_pAADX == NULL || m_pAADX->GetbInited() == FALSE)
		return FALSE;
	m_pAADX->SetVertexB(m_Center, m_Radius, m_Color, m_pVertice, m_VertexCount);
	return TRUE;
}

void AACircleImpl::Draw()
{
	if (m_pVertice == NULL && SetVertex()) NULL;
	if (m_pAADX != NULL)
		m_pAADX->DrawVertex(m_pVertice, m_VertexCount, POINTLIST);
}

Bool AACircleImpl::IsIn(const POINT &p) const
{
	return (pow((S32)abs(p.x - m_Center.x), 2)) + (pow((S32)abs(p.y - m_Center.y), 2)) <= (pow((S32)m_Radius, 2));
}

void AACircleImpl::GetPos(RECT &rc) const
{
	rc.left = m_Center.x - m_Radius;
	rc.right = m_Center.x + m_Radius;
	rc.top = m_Center.y - m_Radius;
	rc.bottom = m_Center.y + m_Radius;
}

//---------------------------------------------------------------------------------------------------------------
//the implementation of AATextImpl
//---------------------------------------------------------------------------------------------------------------
AATextImpl::AATextImpl() : m_pFont(NULL), m_pSprite(NULL), m_pString(NULL), m_pAADX(NULL), m_pTexture(NULL)
{}

AATextImpl::AATextImpl(const TEXTINFO &tinfo, PAADX aadx) : m_Posx(tinfo.x),
												m_Posy(tinfo.y),
												m_Color(tinfo.color),
												m_Format(tinfo.format),
												m_pAADX(aadx),
												m_pFont(NULL),
												m_pSprite(NULL),
												m_pTexture(NULL)
{
	assert(aadx != NULL);

	StringCchCopy(m_BackFile, FILE_PATH_LEN, tinfo.backfile);
	ZeroMemory(&m_FontDesc, sizeof(m_FontDesc));
	StringCchCopy(m_FontDesc.FaceName, 32, tinfo.fontname);
	StringCchCopy(m_FontDesc.FaceName, 32, _T("Arial"));
	m_FontDesc.Width = tinfo.width;
	m_FontDesc.Weight = tinfo.weight;
	m_FontDesc.Height = tinfo.height;
	m_FontDesc.Italic = tinfo.italic;
	m_FontDesc.MipLevels = 1;
	m_FontDesc.CharSet = DEFAULT_CHARSET;

	U32 size = 100;
	//if (FAILED(StringCchLength(tinfo.str, 100, &size)))
	//{
	//	m_pString = new TCHAR[size+1];
	//	StringCchCopyN(m_pString, 100, tinfo.str, 100);
	//	m_pString[size] = '\0';
	//}
	//else
	//{
	//	//if (size > 0)
	//	//{
	//	//	size++;		//for the null character
	//	//	m_pString = new TCHAR[size];
	//	//	StringCchCopy(m_pString, size, tinfo.str);
	//	//}
	//	//else
	//	//{
	//	//	m_pString = NULL;
	//	//}
	//	m_pString = new TCHAR[101];

	//}
	//if (SUCCEEDED(StringCchLength(tinfo.str, 100, &size)))
	//	size = 100;
	m_pString = new TCHAR[size + 1];
	StringCchCopy(m_pString, size, tinfo.str);
	m_pString[size] = '\0';
	ZeroMemory(&m_RectBox, sizeof(m_RectBox));

	m_pAADX->CreateSprite(m_pSprite);
	m_pAADX->CreateAAFont(m_FontDesc, m_pFont);

	if (_tcscmp(m_BackFile, _T("")) != 0)
		m_pAADX->CreateAATexture(m_BackFile, m_pTexture, m_TextureWidth, m_TextureHeight);
	this->SetRectBox();
}

AATextImpl::~AATextImpl()
{
	m_pAADX = NULL;
	if (m_pSprite != NULL)
	{						
		m_pSprite = NULL;	//	the sprite can not be released by textimpl
	}
	if (m_pFont != NULL)
	{
		m_pFont->Release();
		m_pFont = NULL;
	}
	if (m_pTexture != NULL)
	{
		m_pTexture->Release();
		m_pTexture = NULL;
	}
	if (m_pString != NULL)
	{
		delete[] m_pString;
		m_pString = NULL;
	}
}

void AATextImpl::SetString(TCHAR *str)
{
	//if (m_pString != NULL)
	//	delete[] m_pString;
	//m_pString = str;
	StringCchCopy(m_pString, 100, str);
	SetRectBox();
}

Bool AATextImpl::IsIn(const POINT &p) const
{
	return PtInRect(&m_RectBox, p);
}

void AATextImpl::SetRectBox()
{
	if (m_pFont != NULL)
	{
		SIZE size;
		U32 len = 0, hei = 0;
		HDC hdc = m_pFont->GetDC();
		StringCchLength(m_pString, 100, &len);
		GetTextExtentPoint32(hdc, m_pString, len, &size);
		len = size.cx + 8;
		hei = size.cy + 4;
		SetRect(&m_RectBox, m_Posx, m_Posy, m_Posx + len, m_Posy + hei);
	}
	else
	{
		ZeroMemory(&m_RectBox, sizeof(m_RectBox));
	}
}

void AATextImpl::GetRectBox(RECT &rc)
{
	//if (rc.left == 0 && rc.right == 0 && rc.top == 0 && rc.bottom == 0)
	//SetRectBox();
	rc = m_RectBox;
}

void AATextImpl::Draw(U32 color)
{
	U32 len = 0, h = 0;
	if (m_pAADX == NULL)
		return;
	if (m_pSprite == NULL)
		m_pAADX->CreateSprite(m_pSprite);
	if (m_pFont == NULL)
		m_pAADX->CreateAAFont(m_FontDesc, m_pFont);
	if (m_pTexture != NULL)
	{
		StringCchLength(m_pString, 300, &len);
		HDC hdc = m_pFont->GetDC();
		SIZE size;
		GetTextExtentPoint32(hdc, m_pString, len, &size);
		len = size.cx + 8;
		h = (U32)(size.cy + 4);

		static F32 sx, sy;
		sx = (F32)(len) / (F32)m_TextureWidth;
		sy = (F32)(h) / (F32)m_TextureHeight;
		static D3DXMATRIXA16 mat;
		D3DXMatrixScaling( &mat, sx, sy, 0.0f );
		/*D3DXMatrixTransformation2D(&mat, NULL, 0.0f, &D3DXVECTOR2(sx, sy), NULL, 0.0f, &D3DXVECTOR2(m_Posx - 4, m_Posy - 2));*/

		//D3DXVECTOR2 v2i(m_TextureWidth, m_TextureHeight);
		//D3DXVECTOR2 v2o;
		//D3DXVec2TransformCoord(&v2o, &v2i, &mat);
		D3DXVECTOR3 vPos(m_Posx - 4, m_Posy - 2, 1.0f);
		vPos.x /= sx;
		vPos.y /= sy;

		m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
		m_pSprite->SetTransform(&mat);
		m_pSprite->Draw(m_pTexture, NULL, NULL, /*NULL*/&vPos, color);
		m_pSprite->Flush();
		m_pSprite->End();
	}

	RECT rc;
	SetRect(&rc, m_Posx, m_Posy, 0, 0);
	m_pFont->DrawText(NULL, m_pString, -1, &rc, DT_NOCLIP, 0xff000000 | m_Color/*D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)*/);
	
}

//-------------------------------------------------------------------------------------------------------------
//the implementation of AADialImpl
//-------------------------------------------------------------------------------------------------------------
AADialImpl::AADialImpl() : m_pScalFont(NULL), m_pLine(NULL), m_pAADX(NULL), m_pVertexBuffer(NULL)/*, m_pScaleData(NULL)*/
{}

AADialImpl::AADialImpl(const POINT &center, U32 r, U32 color, PAADX aadx, U32 a, U32 b, U32 c) : m_Radius(r),
																								m_Color(color),
																								m_pAADX(aadx),
																								m_a(a),
																								m_b(b),
																								m_c(c)
{
	assert(m_pAADX != NULL);

	m_pAADX->CreateLine(m_pLine);

	D3DXFONT_DESC fontdesc;
	ZeroMemory(&fontdesc, sizeof(fontdesc));
	fontdesc.CharSet = DEFAULT_CHARSET;
	StringCchCopy(fontdesc.FaceName, 33, _T("Arial"));
	fontdesc.Height = 10;
	fontdesc.Weight = 0;
	fontdesc.Width = 5;
	fontdesc.Italic = FALSE;
	fontdesc.MipLevels = 1;
	fontdesc.OutputPrecision = 10;
	m_pAADX->CreateAAFont(fontdesc, m_pScalFont);

	m_Center.x = center.x;
	m_Center.y = center.y;
	m_Radius = r;
	m_PointerPhy = 0.0f;

	m_pAADX->SetBuffer(sizeof(CUSTOMVERTEX) * 3, m_pVertexBuffer);
	//SetScaleData();
}

//void AADialImpl::SetScaleData()
//{
//	m_pScaleData = new SCALEDATA[360];
//	U32 i = 0;
//	const F32 delta = (F32)(D3DX_PI / 180.0f);
//	F32 alpha;
//	U32 fsita;
//
//	for (i = 0; i < 360; i++)
//	{
//		alpha = delta * i;
//		m_pScaleData[i].x1 = m_Center.x + m_Radius * cos(alpha);
//		m_pScaleData[i].y1 = m_Center.y + m_Radius * sin(alpha);
//		if (i % 10 == 0)
//		{
//			m_pScaleData[i].x2 = m_pScaleData[i].x1 + m_a * cos(alpha);
//			m_pScaleData[i].y2 = m_pScaleData[i].y1 + m_a * sin(alpha);
//			m_pScaleData[i].hasscale = TRUE;
//		}
//		else if (i % 5 == 0)
//		{
//			m_pScaleData[i].x2 = m_pScaleData[i].x1 + m_b * cos(alpha);
//			m_pScaleData[i].y2 = m_pScaleData[i].y1 + m_b * sin(alpha);
//			m_pScaleData[i].hasscale = FALSE;
//		}
//		else
//		{
//			m_pScaleData[i].x2 = m_pScaleData[i].x1 + m_c * cos(alpha);
//			m_pScaleData[i].y2 = m_pScaleData[i].y1 + m_c * sin(alpha);
//			m_pScaleData[i].hasscale = FALSE;
//		}
//		fsita = 90 + i;
//		if (fsita >= 360)
//			fsita -= 360;
//		if (fsita < 10)
//			_stprintf(m_pScaleData[i].scalevalue, _T("00%d"), fsita);
//		else if (fsita < 100)
//			_stprintf(m_pScaleData[i].scalevalue, _T("0%d"), fsita);
//		else
//			_stprintf(m_pScaleData[i].scalevalue, _T("%d"), fsita);
//	}
//}

AADialImpl::~AADialImpl()
{
	if (m_pScalFont != NULL)
		m_pScalFont->Release();
	m_pScalFont = NULL;
	m_pLine = NULL;		//m_pLine can not release here
	m_pAADX = NULL;		//the same to the above
	//if (m_pScaleData != NULL)
	//	delete[] m_pScaleData;
	//m_pScaleData = NULL;
	if (m_pVertexBuffer != NULL)
		m_pVertexBuffer->Release();
	m_pVertexBuffer = NULL;
}

void AADialImpl::Draw(F32 angle)	//the angle is in degree
{
	F32 fsita = 90.0f + angle;
	while (fsita >= 360.0f)
		fsita -= 360.0f;
	while (fsita < 0)
		fsita += 360.0f;
	U32 scale = (U32)ceil(fsita);
	static const F32 delta = (F32)(D3DX_PI / 180.0f);
	F32 alpha = (F32)((scale - fsita) * delta);
	static const F32 dpia = 2 * D3DX_PI;
	static const F32 beta1 = (F32)(0.25 * D3DX_PI);
	static const F32 beta2 = (F32)(0.75 * D3DX_PI);
	static const F32 beta3 = (F32)(1.25 * D3DX_PI);
	static const F32 beta4 = (F32)(1.75 * D3DX_PI);
	
	static D3DXVECTOR2 Line[2];
	static TCHAR scalestr[10];
	static Bool drawscale = FALSE;
	static RECT rc;

	m_pLine->SetWidth(1);
	m_pLine->SetAntialias(TRUE);
	if (FAILED(m_pLine->Begin()))
		return;
	if (FAILED(m_pLine->Draw(Line, 2, 0xff000000 | m_Color)))
		return;
	while(alpha <= dpia)
	{
		Line[0].x = m_Center.x + m_Radius * cos(alpha);
		Line[0].y = m_Center.y + m_Radius * sin(alpha);
		if (scale % 10 == 0)
		{
            Line[1].x = m_Center.x + (m_Radius + m_a) * cos(alpha);
			Line[1].y = m_Center.y + (m_Radius + m_a) * sin(alpha);
			drawscale = TRUE;
		}
		else if (scale % 5 == 0)
		{
			Line[1].x = Line[0].x + m_b * cos(alpha);
			Line[1].y = Line[0].y + m_b * sin(alpha);
			drawscale = FALSE;
		}
		else
		{
			Line[1].x = Line[0].x + m_c * cos(alpha);
			Line[1].y = Line[0].y + m_c *sin(alpha);
			drawscale = FALSE;
		}
		m_pLine->Draw(Line, 2, m_Color);
		
		if (drawscale)
		{
			if (scale < 10)
				_stprintf(scalestr, _T("00%d"), scale);
			else if (scale < 100)
				_stprintf(scalestr, _T("0%d"), scale);
			else
				_stprintf(scalestr, _T("%d"), scale);

			if (alpha <= beta1 || alpha >= beta4)
			{
				rc.left = (U32)Line[1].x + 5;
				rc.top = (U32)Line[1].y - 5;
				rc.right = rc.left + 10;
				rc.bottom = rc.top + 10;
				m_pScalFont->DrawText(NULL, scalestr, -1, &rc, DT_NOCLIP | DT_LEFT, 0xff000000 | m_Color);
			}
			else if (alpha > beta1 && alpha <= beta2)
			{
				rc.left = (U32)Line[1].x - 10;
				rc.top = (U32)Line[1].y ;
				rc.bottom = (U32)Line[1].y + 10;
				rc.right = rc.left + 20;
				m_pScalFont->DrawText(NULL, scalestr, -1, &rc, DT_NOCLIP | DT_TOP | DT_CENTER , 0xff000000 | m_Color);
			}
			else if (alpha > beta2 && alpha <= beta3)
			{
				rc.left = (U32)Line[1].x - 20;
				rc.top = (U32)Line[1].y - 5;
				rc.right = (U32)Line[1].x - 5;
				rc.bottom = (U32)Line[1].y + 18;
				m_pScalFont->DrawText(NULL, scalestr, -1, &rc, DT_NOCLIP | DT_RIGHT , 0xff000000 | m_Color);
			}
			else if (alpha > beta3 && alpha <= beta4)
			{
				rc.left = (U32)Line[1].x - 10;
				rc.top = (U32)Line[1].y - 20;
				rc.right = (U32)Line[1].x + 10;
				rc.bottom = (U32)Line[1].y;
				m_pScalFont->DrawText(NULL, scalestr, -1, &rc, DT_NOCLIP | DT_BOTTOM | DT_CENTER, 0xff000000 | m_Color);
			}
		}
		
		alpha += delta;
		scale++;
		if (scale >= 360)
			scale -= 360;
	}
	m_pLine->End();

	static const F32 pointerstep = 3 * delta;
	static const F32 pointerwidth = 8 * delta;
	while(m_PointerPhy >= dpia)
		m_PointerPhy -= dpia;
	while(m_PointerPhy < 0)
		m_PointerPhy += dpia;
	static CUSTOMVERTEX pointer[3];
	pointer[0].x = m_Center.x;
	pointer[0].y = m_Center.y;
	pointer[0].z = 0;
	pointer[0].rhw = 1.0f;
	pointer[0].color = 0x88008800;

	pointer[1].x = m_Center.x + m_Radius * cos(m_PointerPhy);
	pointer[1].y = m_Center.y + m_Radius * sin(m_PointerPhy);
	pointer[1].z = 0;
	pointer[1].rhw = 1.0f;
	pointer[1].color = 0x00000000;

	pointer[2].x = m_Center.x + m_Radius * cos(m_PointerPhy + pointerwidth);
	pointer[2].y = m_Center.y + m_Radius * sin(m_PointerPhy + pointerwidth);
	pointer[2].z = 0;
	pointer[2].rhw = 1.0f;
	pointer[2].color = 0xaa00ff00;
	m_PointerPhy += pointerstep;

	static void **data;
	m_pVertexBuffer->Lock(0, sizeof(pointer), (void**)&data, NULL);
	memcpy(data, pointer, sizeof(pointer));
	m_pVertexBuffer->Unlock();
	m_pAADX->DrawVertex(m_pVertexBuffer, 1, TRIANGLEFAN);

	
	//F32 fsita = (angle > 0) ? (angle + 0.5f) : (angle - 0.5f);
	//while (fsita >= 360.0f)
	//	fsita -= 360.0f;
	//while (fsita < 0)
	//	fsita += 360.0f;
	//U32 offset = (U32)fsita;
	//U32 start = offset;
	//D3DXVECTOR2 line[2];
	//RECT rc;

	//m_pLine->Begin();
	//m_pLine->SetAntialias(TRUE);
	//m_pLine->SetWidth(1);
	//do{
	//	line[0].x = m_pScaleData[start].x1;
	//	line[0].y = m_pScaleData[start].y1;
	//	line[1].x = m_pScaleData[start].x2;
	//	line[1].y = m_pScaleData[start].y2;
	//	m_pLine->Draw(line, 2, 0xff000000 | m_Color);

	//	if (m_pScaleData[start].hasscale == TRUE)
	//	{
	//		if (start >= 315 || start <= 45)
	//		{
	//			rc.left = (U32)line[1].x + 3;
	//			rc.top = (U32)line[1].y - 10;
	//			rc.right = (U32)line[1].x + 23;
	//			rc.bottom = (U32)line[1].y + 10;
	//			m_pScalFont->DrawText(NULL, m_pScaleData[start].scalevalue, -1, &rc, DT_NOCLIP | DT_LEFT | DT_VCENTER, 0xff000000 | m_Color);
	//		}
	//		else if (start > 45 && start <= 135)
	//		{
	//			rc.left = line[1].x - 10;
	//			rc.top = line[1].y + 3;
	//			rc.right = line[1].x + 10;
	//			rc.bottom = line[1].y + 23;
	//			m_pScalFont->DrawText(NULL, m_pScaleData[start].scalevalue, -1, &rc, DT_NOCLIP | DT_TOP | DT_CENTER, 0xff000000 | m_Color);
	//		}
	//		else if (start > 135 && start <= 225)
	//		{
	//			rc.left = line[1].x - 23;
	//			rc.top = line[1].y - 10;
	//			rc.right = line[1].x - 3;
	//			rc.bottom = line[1].y + 10;
	//			m_pScalFont->DrawText(NULL, m_pScaleData[start].scalevalue, -1, &rc, DT_NOCLIP | DT_RIGHT | DT_VCENTER, 0xff000000 | m_Color);
	//		}
	//		else
	//		{
	//			rc.left = line[1].x - 10;
	//			rc.top = line[1].y - 23;
	//			rc.right = line[1].x + 10;
	//			rc.bottom = line[1].y - 3;
	//			m_pScalFont->DrawText(NULL, m_pScaleData[start].scalevalue, -1, &rc, DT_NOCLIP | DT_BOTTOM |DT_CENTER, 0xff000000 | m_Color);
	//		}
	//	}

	//	start++;
	//	if (start >= 360)
	//		start -= 360;
	//}while (start != offset);

	//m_pLine->End();
}

//---------------------------------------------------------------------------------------------------------
//the implementation of AAShipImpl
//---------------------------------------------------------------------------------------------------------
AAShipImpl::AAShipImpl()
{
	m_SelectedTexture = NULL;
	m_UnselectedTexture = NULL;
	m_STextureWidth = 0;
	m_STextureHeight = 0;
	m_UTextureWidth = 0;
	m_UTextureHeight = 0;
	m_Sprite = NULL;
}

AAShipImpl::AAShipImpl(AADX *aadx, const TCHAR *selectedfile, const TCHAR *unselectedfile)
{
	m_SelectedTexture = NULL;
	m_UnselectedTexture = NULL;
	m_Sprite = NULL;
	aadx->CreateAATexture(selectedfile, m_SelectedTexture, m_STextureWidth, m_STextureHeight);
	aadx->CreateAATexture(unselectedfile, m_UnselectedTexture, m_UTextureWidth, m_UTextureHeight);
	aadx->CreateSprite(m_Sprite);
}

AAShipImpl::~AAShipImpl()
{
	if (m_SelectedTexture != NULL)
	{
		m_SelectedTexture->Release();
		m_SelectedTexture = NULL;
	}
	if (m_UnselectedTexture != NULL)
	{
		m_UnselectedTexture->Release();
		m_UnselectedTexture = NULL;
	}
	if (m_Sprite != NULL)
		m_Sprite = NULL;
}

void AAShipImpl::DrawSelected(const RECT &pos)
{
	F32 sx = (F32)(pos.right - pos.left) / (F32)(m_STextureWidth);
	F32 sy = (F32)(pos.bottom - pos.top) / (F32)(m_STextureHeight);
	D3DXMATRIXA16 mat;

	D3DXMatrixScaling(&mat, sx, sy, 0.0f);
	D3DXVECTOR3 vPos(pos.left, pos.top, 1.0f);
	vPos.x /= sx;
	vPos.y /= sy;
	m_Sprite->Begin(D3DXSPRITE_ALPHABLEND);
	m_Sprite->SetTransform(&mat);
	m_Sprite->Draw(this->m_SelectedTexture, NULL, NULL, &vPos, 0xffaaff00);
	m_Sprite->Flush();
	m_Sprite->End();
}

void AAShipImpl::DrawUnselected(const RECT &pos)
{
	F32 sx = (F32)(pos.right - pos.left) / (F32)(m_UTextureWidth);
	F32 sy = (F32)(pos.bottom - pos.top) / (F32)(m_UTextureHeight);
	D3DXMATRIXA16 mat;

	D3DXMatrixScaling(&mat, sx, sy, 0.0f);
	D3DXVECTOR3 vPos(pos.left, pos.top, 1.0f);
	vPos.x /= sx;
	vPos.y /= sy;
	m_Sprite->Begin(D3DXSPRITE_ALPHABLEND);
	m_Sprite->SetTransform(&mat);
	m_Sprite->Draw(this->m_UnselectedTexture, NULL, NULL, &vPos, 0xaaaaff00);
	m_Sprite->Flush();
	m_Sprite->End();
}