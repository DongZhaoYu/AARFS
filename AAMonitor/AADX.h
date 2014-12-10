/*
*	@file: AADX.h
*	@brief:	to encasulate the directx9.0 for AAMonitor
*	@author: dongzhaoyu
*	@date: 2008-4-15
*
*	Copyright (c) 2008 CTEC Xi'an JiaoTong University
*/

#if _MSC_VER > 1200
#pragma once 
#define _AADX_H
#endif

#include <d3d9.h>
#include <d3dx9.h>
#include <D3dx9Core.h>
#pragma warning (disable : 4996)
#include <strsafe.h>
#pragma warning (default : 4996)
#include <windows.h>

#include "..\AACommon\AAStdtype.h"
#include "..\AACommon\AAPredefine.h"

#ifndef FILE_PATH_LEN
#define FILE_PATH_LEN 200
#endif

typedef LPDIRECT3DVERTEXBUFFER9 AAPVERTEXBUFFER;
typedef ID3DXFont *AAPFONT;
typedef struct CUSTOMVERTEX
{
	F32 x, y, z, rhw;
	U32 color;
}CUSTOMVERTEX, *LPCUSTOMVERTEX;
typedef enum SHAPE
{
	CIRCLE,
	RECTANGULAR
}SHAPE;
typedef enum _DRAWSTYLE
{
	POINTLIST,
	LINELIST,
	POLYLINE,
	TRIANGLELIST,
	TRIANGLEFAN
}DRAWSTYLE;

#define AAD3DCUSTOMVERTEX_FVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

class AADX
{
public:
	AADX();
	AADX(HWND);
	~AADX();	//this class will have no subclass

	Bool Init3D(HWND = NULL, Bool = FALSE);
	void DrawAText(const RECT &rc, U32 color, const U8 *text);
	void DrawVertex(const AAPVERTEXBUFFER buffer, U32 count, DRAWSTYLE style = LINELIST) const;
	void BeginRender(U8 = 0, U8 = 0, U8 = 0);
	void EndRender();
	Bool SetVertex(const POINT &p, U32 r, U32 color, AAPVERTEXBUFFER &buffer, U32 &count) const;	//the primitive count
	Bool SetVertex(const RECT &rc, U32 color, AAPVERTEXBUFFER &buffer, U32 &count) const;
	Bool SetVertexB(const POINT &p, U32 r, U32 color, AAPVERTEXBUFFER &buffer, U32 &count) const;	//the bresenham algorithm
	Bool SetVertex(const POINT &start, const POINT &end, U32 color, AAPVERTEXBUFFER &buffer) const;
	Bool CreateSprite(LPD3DXSPRITE &) const;
	Bool CreateAAFont(const D3DXFONT_DESC &fontdesc, LPD3DXFONT &font) const;
	Bool CreateAATexture(U32 sourceid, LPDIRECT3DTEXTURE9 &texture) const;
	Bool CreateAATexture(LPCTSTR filename, LPDIRECT3DTEXTURE9 &texture, U32 &width, U32 &height) const;
	void CreateLine(LPD3DXLINE &line);
	Bool SetBuffer(U32 size, AAPVERTEXBUFFER &buffer) const;
	Bool SetVertex(CUSTOMVERTEX *vertex, U32 vertexlen, AAPVERTEXBUFFER buffer);
	void Cleanup();

	GET_ACCESSOR(Bool, bInited)
	GET_ACCESSOR(Bool, bBegin)
	GET_ACCESSOR(HWND, hWnd)

private:
	HWND m_hWnd;	//the window to draw on
	LPDIRECT3D9 m_pD3D;
	LPDIRECT3DDEVICE9 m_pD3DDevice;
	LPD3DXSPRITE m_pSprite;
	LPD3DXLINE m_pLine;

	Bool m_bInited;
	Bool m_bBegin;
	U32 m_WWidth;
	U32 m_WHeight;
};
typedef AADX *PAADX;

class AALineImpl
{
public:
	typedef AALineImpl *PAALineImpl;

	AALineImpl();
	AALineImpl(const POINT &start, const POINT &end, U32 color, PAADX aadx);

	virtual ~AALineImpl();
	virtual void Draw();

private:
	POINT m_Start;
	POINT m_End;
	U32 m_Color;
	PAADX m_pAADX;
	AAPVERTEXBUFFER m_pVertice;
};

class AARectImpl
{
public:
	AARectImpl(const PAADX);
	AARectImpl(const PAADX, U32 color, const RECT&);
	virtual ~AARectImpl();

	void Draw();
	Bool IsIn(const POINT&) const;
	void GetPos(RECT &rc) const;

	GET_ACCESSOR(RECT, RC)
	GET_ACCESSOR(U32, Color)
	GET_ACCESSOR(PAADX, pAADX)

protected:
	Bool SetVertex();

private:
	RECT m_RC;
	U32 m_Color;
	AAPVERTEXBUFFER m_pVertice;
	U32 m_VertexCount;	//primitive count
	PAADX m_pAADX;
};
typedef AARectImpl *PAARectImpl;

class AACircleImpl
{
public:
	typedef AACircleImpl *PAACircleImpl;

	AACircleImpl();
	AACircleImpl(const POINT &p, U32 r, U32 c, const PAADX aadx);
	virtual ~AACircleImpl();

	void Draw();
	Bool IsIn(const POINT&) const;
	void GetPos(RECT &rc) const;

	GET_ACCESSOR(POINT, Center)
	GET_ACCESSOR(U32, Radius)
	GET_ACCESSOR(U32, Color)

protected:
	Bool SetVertex();

private:
	POINT m_Center;
	U32 m_Radius;
	U32 m_Color;
	AAPVERTEXBUFFER m_pVertice;
	U32 m_VertexCount;
	PAADX m_pAADX;
};

class AATextImpl
{
public:
	typedef AATextImpl *PAATextImpl;
	typedef struct _TextInfo
	{
		U32 x;
		U32 y;
		U32 weight;
		U32 height;
		U32 width;
		U32 color;
		U32 format;
		Bool italic;
		LPCTSTR fontname;
		TCHAR *str;
		TCHAR backfile[FILE_PATH_LEN];
	}TEXTINFO;

	AATextImpl();
	AATextImpl(const TEXTINFO&, const PAADX);
	virtual ~AATextImpl();

	void Draw(U32 color = 0xaaffffff);
	Bool IsIn(const POINT&) const;
	void SetRectBox();
	void GetRectBox(RECT &rc);

	void SetPosx(U32 pos) { m_Posx = pos; SetRectBox();}
	void SetPosy(U32 pos) { m_Posy = pos; SetRectBox();}
	SET_GET_ACCESSOR(U32, Color)
	SET_GET_ACCESSOR(U32, Format)
	
	void SetString(TCHAR *str);

private:
	U32 m_Posx;
	U32 m_Posy;
	U32 m_Color;
	U32 m_Format;
	TCHAR m_BackFile[FILE_PATH_LEN];
	TCHAR* m_pString;
	D3DXFONT_DESC m_FontDesc;
	LPD3DXFONT m_pFont;
	LPD3DXSPRITE m_pSprite;
	LPDIRECT3DTEXTURE9 m_pTexture;
	U32 m_TextureWidth;
	U32 m_TextureHeight;
	PAADX m_pAADX;
	RECT m_RectBox;
};

class AADialImpl
{
public:
	typedef struct _scaledata{
		F32 x1, y1, x2, y2;
		TCHAR scalevalue[4];
		Bool hasscale;
	}SCALEDATA;

	AADialImpl();
	AADialImpl(const POINT &center, U32 r, U32 color, PAADX aadx, U32 a = 18, U32 b = 12, U32 c = 6);

	virtual ~AADialImpl();
	virtual void Draw(F32 angle);
	//virtual void SearchDraw(F32 angle) const;

protected:
	//void SetScaleData();

private:
	POINT m_Center;
	U32 m_Radius;
	U32 m_Color;
	U32 m_a;
	U32 m_b;
	U32 m_c;
	LPD3DXFONT m_pScalFont;
	LPD3DXLINE m_pLine;
	PAADX m_pAADX;
	AAPVERTEXBUFFER m_pVertexBuffer;
	F32 m_PointerPhy;
	//SCALEDATA *m_pScaleData;
};

class AAShipImpl
{
public:
	AAShipImpl();
	AAShipImpl(AADX *aadx, const TCHAR *selectedfile, const TCHAR *unselectedfile);

	~AAShipImpl();
	void DrawSelected(const RECT &pos);
	void DrawUnselected(const RECT &pos);

	GET_ACCESSOR(U32, STextureWidth)
	GET_ACCESSOR(U32, STextureHeight)

private:
	LPDIRECT3DTEXTURE9 m_SelectedTexture;
	LPDIRECT3DTEXTURE9 m_UnselectedTexture;
	U32 m_STextureWidth;
	U32 m_STextureHeight;
	U32 m_UTextureWidth;
	U32 m_UTextureHeight;
	LPD3DXSPRITE m_Sprite;
};