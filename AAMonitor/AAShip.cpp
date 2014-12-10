/*
*	@file: AAShip.cpp
*	@brief: implementation of the AAShip.h
*	@author: dongzhaoyu
*	@date: 2008-6-2
*
*	Copyright (C) CTEC Xi'an JiaoTong University
*/

#include "AAShip.h"
#define DISSQUARE(x1, y1, x2, y2) (((x1) - (x2)) * ((x1) - (x2)) + ((y1) - (y2))*((y1) - (y2)))

//-------------------------------------------------------------------------------------------------
//implementation of AAShipManager
//-------------------------------------------------------------------------------------------------
AAShipManager::AAShipManager() : m_Impl(NULL), m_CurrentSelectedShip(-1), m_NotifyWin(NULL)
{}

AAShipManager::AAShipManager(AADX *aadx, S32 shipcount, F64 radarrange, const TCHAR *selectedfile, const TCHAR *unselectedfile)
{
	m_RadarRange = radarrange;
	m_Impl = new AAShipImpl(aadx, selectedfile, unselectedfile);
	Init(shipcount);
	m_CurrentSelectedShip = -1;
	m_NotifyWin = NULL;
}

AAShipManager::~AAShipManager()
{
	if (m_Impl)
	{
		delete m_Impl;
		m_Impl = NULL;
	}
}

void AAShipManager::Init(S32 count)
{
	AAShip ship;
	ZeroMemory(&ship, sizeof(ship));
	for (S32 i = 0; i < count; i++)
	{
		m_Ships.Add(ship);
	}
}

void AAShipManager::UpdataShip(const AANotifyMsg *msg, const POINT &centor, S32 radius, F32 angle)
{
	F64 scale = msg->m_ShipPos.m_Distance / m_RadarRange;
	S32 distance = S32(scale * radius);
	F32 dangle = F32(msg->m_ShipPos.m_Angle + angle * DELTA);
	S32 x = centor.x + S32(cos(F32(dangle)) * distance);
	S32 y = centor.y - S32(sin(F32(dangle)) * distance);
	S32 offset = (S32)((F32)radius / 10.0f) >> 1;
	S32 leftx = m_Ships[msg->m_ShipPos.m_ShipID].m_Rect.left = x - offset;
	S32 rightx = m_Ships[msg->m_ShipPos.m_ShipID].m_Rect.right = x + offset;
	S32 bottomy = m_Ships[msg->m_ShipPos.m_ShipID].m_Rect.top = y - offset;
	S32 topy = m_Ships[msg->m_ShipPos.m_ShipID].m_Rect.bottom = y + offset;
	S32 rs = radius * radius;

	if (DISSQUARE(leftx, topy, centor.x, centor.y) < rs &&
		DISSQUARE(rightx, topy, centor.x, centor.y) < rs &&
		DISSQUARE(rightx, bottomy, centor.x, centor.y) < rs &&
		DISSQUARE(leftx, bottomy, centor.x, centor.y) < rs)
	{
		m_Ships[msg->m_ShipPos.m_ShipID].m_TTL = TOTALTTL;
	}
	else
	{
		m_Ships[msg->m_ShipPos.m_ShipID].m_TTL = 0;		//we think the ship has run out of the range
	}
}

void AAShipManager::DrawShip()
{
	S32 size = m_Ships.GetSize();
	static S32 notifytimes = 5;
	for (S32 i = 0; i < size; i++)
	{
		if (m_Ships[i].m_TTL > 0)
		{
			if (m_Ships[i].m_HasFocus == FALSE)
			{
				m_Impl->DrawUnselected(m_Ships[i].m_Rect);
			}
			else
			{
				m_Impl->DrawSelected(m_Ships[i].m_Rect);
			}
			m_Ships[i].m_TTL--;
		}
	}
	if (notifytimes-- <= 0)	
	{
		NotifySelected();
		notifytimes = 5;
	}
}

Bool AAShipManager::HandleMouse(U32 Msg, WPARAM wparam, LPARAM lparam)
{
	POINT p;
	p.x = LOWORD(lparam);
	p.y = HIWORD(lparam);

	switch(Msg)
	{
	case WM_LBUTTONDOWN:
		{
			S32 size = m_Ships.GetSize();
			for (S32 i = size - 1; i >= 0; i--)		//search inversely
			{
				if (PtInRect(&(m_Ships[i].m_Rect), p))
				{
					if (m_CurrentSelectedShip >= 0 && m_CurrentSelectedShip < size)
						m_Ships[m_CurrentSelectedShip].m_HasFocus = FALSE;
					this->m_CurrentSelectedShip = i;
					m_Ships[i].m_HasFocus = TRUE;

					//notify the user input to AARFSView
					NotifySelected();
					return TRUE;
				}
			}
			break;
		}
	}
	return FALSE;
}