/*	
*	@file: AAGUI.cpp
*	@brife: the implementation of gui controls used in AARFS
*	@author: dongzhaoyu
*	@date: 2008-4-27
*
*	Copyright (C) CTEC Xi'an JiaoTong University
*/

#include "AAGUI.h"
#pragma warning (disable : 4005)
#include <tchar.h>
#pragma warning (default : 4005)

//----------------------------------------------------------------------------------------------------------------
//the implementation of AAEventHandler
//----------------------------------------------------------------------------------------------------------------
AAEventHandler::AAEventHandler(Bool a, Bool b, Bool c)
{
	m_bMouseOver = a;
	m_bHasFocus = b;
	m_bEnable = c;
}

//----------------------------------------------------------------------------------------------------------------
//the implementation of AAGUIControl
//----------------------------------------------------------------------------------------------------------------
AAGUIControl::AAGUIControl(PAADX aadx, U32 id) : AAEventHandler(FALSE, FALSE, TRUE)
{
	m_X = 0;
	m_Y = 0;
	m_Height = 0;
	m_Width = 0;

	m_Type = AAGUIT_BUTTON;
	m_ID = id;
	m_pAADX = aadx;
	m_pUserData = NULL;
	
	ZeroMemory(&m_RectBox, sizeof(m_RectBox));
}

AAGUIControl::~AAGUIControl()
{
	if (m_pUserData != NULL)
		delete[] m_pUserData;
	m_pUserData = NULL;

	for(S32 i = 0; i < m_lElements.GetSize(); i++)
		delete m_lElements.GetAt(i);
}

//-------------------------------------------------------------------------------------------------------------------
//the implementation of AAGUIStatic
//-------------------------------------------------------------------------------------------------------------------
AAGUIStatic::AAGUIStatic() : AAGUIControl(NULL)
{
	m_Type = AAGUIT_STATIC;
}

AAGUIStatic::AAGUIStatic(AAText::INFO &tinfo, PAADX aadx, U32 id) : AAGUIControl(aadx, id)
{
	this->m_Type = AAGUIT_STATIC;
	this->m_Height = tinfo.height;
	this->m_Width = tinfo.width;
	this->m_X = tinfo.x;
	this->m_Y = tinfo.y;
	this->m_lElements.Insert(0, new AAText(tinfo, "", aadx));
	this->m_Color.a = F32(tinfo.color >> 24);
	this->m_Color.r = F32((tinfo.color & 0x00ff0000) >> 16);
	this->m_Color.g = F32((tinfo.color & 0x0000ff00) >> 8);
	this->m_Color.b = F32(tinfo.color & 0x000000ff);
	
	/*SetRect(&m_RectBox, m_X, m_Y, m_X + m_Width, m_Y + m_Height);*/
	Element *e = m_lElements.GetAt(0);
	e->GetPos(m_RectBox);
}

void AAGUIStatic::Update()
{
	Element *e = m_lElements.GetAt(0);
	e->SetPos(m_X, m_Y);
	e->GetPos(m_RectBox);
}

void AAGUIStatic::SetUserData(void *data)
{
	m_pUserData = data;
	m_lElements.GetAt(0)->SetData(data);
	Update();
}

void AAGUIStatic::Render()
{
	m_lElements.GetAt(0)->Draw();
}

//-----------------------------------------------------------------------------------------------------------
// the implement of AAGUIButton
//-----------------------------------------------------------------------------------------------------------
AAGUIButton::AAGUIButton() : AAGUIStatic()
{
	this->m_Type = AAGUIT_BUTTON;
	m_bPressed = FALSE;
	//m_ColorA = 0x00aabb00;
	//m_ColorB = 0x00aabbff;
	//m_Coefficient = 0.8f;
}

AAGUIButton::AAGUIButton(AAText::INFO &tinfo, PAADX aadx, U32 id) : AAGUIStatic(tinfo, aadx, id)
{
	this->m_Type = AAGUIT_BUTTON;
	m_bPressed = FALSE;
	//ZeroMemory(&m_ColorA, sizeof(m_ColorA));
	//m_ColorB.a = 255;
	//m_ColorB.r = 121;
	//m_ColorB.g = 124;
	//m_ColorB.b = 131;
	//m_ColorB = 0xffffffff;
}

AAGUIButton::~AAGUIButton()
{}

void AAGUIButton::Render()
{
	D3DXCOLOR color = m_Color | 0x8800ff00;
	S32 xoffset = 0, yoffset = 0;

	if (m_bPressed)
	{
		//D3DXColorLerp(&color, &m_ColorA, &m_ColorB, m_Coefficient);
		color = 0x4400ff00;
		xoffset = 1;
		yoffset = 2;
	}
	else if (m_bMouseOver)
	{
		//D3DXColorLerp(&color, &m_ColorA, &m_ColorB, m_Coefficient * 0.5f);
		color = 0xff00ff00;
		xoffset = -1;
		yoffset = -2;
	}

	Element *e = m_lElements.GetAt(0);
	//e->SetColor(D3DCOLOR_ARGB((U32)color.a, (U32)color.r, (U32)color.g, (U32)color.b));
	RECT rc;
	e->GetPos(rc);
	OffsetRect(&rc, xoffset, yoffset);
	//e->SetPos(rc.left, rc.top);
	//e->SetPos(4, 2);
	//Update();
	SetPosition(rc.left, rc.top);
	e->Draw(/*D3DCOLOR_ARGB((U32)color.a, (U32)color.r, (U32)color.g, (U32)color.b)*/color);

	OffsetRect(&rc, -xoffset, -yoffset);
	SetPosition(rc.left, rc.top);
}

Bool AAGUIButton::HandleMouse(U32 Msg, /*POINT point, */WPARAM wparam, LPARAM lparam)
{
	POINT point;
	point.x = LOWORD(lparam);
	point.y = HIWORD(lparam);

	switch(Msg)
	{
	case WM_LBUTTONDOWN:
		if (ContainPoint(point))
		{
			m_bPressed = TRUE;
			PostMessage(m_pAADX->GethWnd(), WM_GUIEVENT, m_ID, 0);
			return TRUE;
		}
		break;
	case WM_MOUSEMOVE:
		if (ContainPoint(point))
		{
			this->OnMouseEnter();
			return TRUE;
		}
		else
		{
			this->OnMouseLeave();
			m_bPressed = FALSE;
		}
		break;
	case WM_LBUTTONUP:
		if (ContainPoint(point))
		{
			m_bPressed = FALSE;
			return TRUE;
		}
	default:
		break;
	}
	return FALSE;
}