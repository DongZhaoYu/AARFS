/*
*	@file: AAShip.h
*	@brief: definition of the ship to show on radar monitor
*	@author: dongzhaoyu
*	@date: 2008-6-2
*
*	Copyright (C) CTEC Xi'an JiaoTong University
*/

#ifndef _AASHIP_H
#define _AASHIP_H

#include <windows.h>
#include "../AACommon/AANotifyMessage.h"
#include "AADX.h"
#pragma warning (disable : 4005)
#include <tchar.h>
#pragma warning (default : 4005)
#include "AAGUI.h"

#define TOTALTTL 20
#define DELTA (3.1415926f / 180.0f)

typedef struct _AAShip
{
	RECT m_Rect;			//the rectangular the ship occupied on the screen
	S32 m_TTL;				//the time to update the ship state
	Bool m_HasFocus;		//if the ship was selected
}AAShip;

class AAShipManager : public AAEventHandler
{
public:
	typedef CGrowableArray<AAShip> ShipArray;

	AAShipManager();
	AAShipManager(AADX *aadx, S32 shipcount, F64 radarrange, const TCHAR *selectedfile = _T(".\\res\\selected_ship.dib"), const TCHAR *unselectedfile = _T(".\\res\\unselected_ship.dib"));

	~AAShipManager();
	void UpdataShip(const AANotifyMsg *msg, const POINT &centor, S32 radius, F32 angle);
	void DrawShip();
	virtual Bool HandleMouse(U32 Msg, /*POINT point, */WPARAM wparam, LPARAM lparam);

	SET_GET_ACCESSOR(HWND, NotifyWin)

private:
	void Init(S32 count);
	void NotifySelected()
	{
		if (m_NotifyWin && m_CurrentSelectedShip >= 0 && m_CurrentSelectedShip < m_Ships.GetSize()) 
		::PostMessage(m_NotifyWin, WM_AASHIPSELECTED, m_CurrentSelectedShip, 0);
	}

	F64 m_RadarRange;
	AAShipImpl *m_Impl;
	ShipArray m_Ships;
	S32 m_CurrentSelectedShip;			//the index of the ship currently selected
	HWND m_NotifyWin;					//notify the window of the user input
};

#endif