/*
* @file: AAMonitorImpl.h
* @brief: Radar monitor implementation
* @author: dongzhaoyu
* @date: 2008-4-15
*
* Copyright (c) 2008 CTEC Xi'an JiaoTong University
*/

#if _MSC_VER > 1200
#pragma once
#define _AAMONITORIMPL_H
#endif

#include "AAinterface.h"
#include "AADX.h"
#include "..\AACommon\AAPredefine.h"
#include "..\AACommon\AANotifyMessage.h"
#include "AAElements.h"
#include "AAGUI.h"
#include "time.h"
#include "AAShip.h"

class AAMonitorImpl : public AAMonitor, public AAEventHandler
{
public:
	typedef AAText::INFO AATEXTINFO;

	AAMonitorImpl();
	AAMonitorImpl(HWND, U32 = 1);

	virtual ~AAMonitorImpl();
	virtual void ShowMonitor();
	virtual void OnUpdateData(AANotifyMsg*);
	virtual void OnRcvFusionPara(AANotifyMsg*);
	virtual Bool HandleMouse(U32 Msg, WPARAM wparam, LPARAM lparam);
	virtual Bool AddRect(const RECT &rc, const U32 color, const char *dscp);
	virtual Bool AddCircle(const POINT &center, const U32 r, const U32 color, const char *dscp);
	virtual Bool AddText(AATEXTINFO &textinfo, const char *title);
	virtual Bool AddLine(const POINT &start, const POINT &end, U32 color, const char *dscp);
	virtual Bool AddDial(Element *element, U32 color, F32 angle);
	virtual Bool AddStatic(AATEXTINFO &textinfo, S32 id);
	virtual Bool AddButton(AATEXTINFO &textinfo, S32 id);
	virtual void SetGUIText(S32 id, TCHAR *text);
	virtual void SetEleAngle(F32 angle);
	void Init();

	SET_GET_ACCESSOR(HWND, hWnd)
	SET_GET_ACCESSOR(MouseHandler, hMHandler)
	SET_GET_ACCESSOR(KeyboardHandler, hKeyHandler)
	SET_GET_ACCESSOR(U32, Timespan)
	GET_ACCESSOR(PAADX, pAADX)
	SET_GET_ACCESSOR(U32, Radius)
	SET_GET_ACCESSOR(POINT, Centor)
	SET_GET_ACCESSOR(F32, Angle)
	SET_GET_ACCESSOR(S32, ShipCount)
	SET_GET_ACCESSOR(F64, RadarRange)

private:
	HWND m_hWnd;
	S32 m_ShipCount;
	MouseHandler m_hMHandler;
	KeyboardHandler m_hKeyHandler;
	PAADX m_pAADX;
	Element::Element_List_t m_lElements;
	CGrowableArray<AAGUIControl*> m_lControls;
	AAShipManager *m_ShipManager;
	U32 m_Timespan;	//in second
	S64 m_CurTime;	//current time
	POINT m_Centor;
	U32 m_Radius;
	F32 m_Angle;
	F64 m_RadarRange;
};