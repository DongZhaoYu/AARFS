/*
* @file: AAMonitorImpl.cpp
* @brief: Radar monitor implemation
* @author: dongzhaoyu
* @date: 2008-4-15
*
* Copyright (c) 2008 CTEC Xi'an JiaoTong University
*/

#include "AAMonitorImpl.h"
#pragma warning (disable : 4005)
#include <tchar.h>
#pragma warning (default : 4005)

AAMonitorImpl::AAMonitorImpl() : AAEventHandler(), m_hWnd(NULL), m_hMHandler(NULL), m_hKeyHandler(NULL), m_pAADX(NULL), m_Timespan(1), m_ShipManager(NULL){}

AAMonitorImpl::AAMonitorImpl(HWND hWnd, U32 timerspan) : AAEventHandler(), m_hWnd(hWnd), m_hMHandler(NULL), m_hKeyHandler(NULL), m_ShipManager(NULL), m_pAADX(NULL), m_Timespan(timerspan)
{
	m_pAADX = new AADX(hWnd);
	m_pAADX->Init3D();
}

AAMonitorImpl::~AAMonitorImpl()
{
	if (m_pAADX != NULL)
	{
		delete m_pAADX;
		m_pAADX = NULL;
	}
	if (m_ShipManager != NULL)
	{
		delete m_ShipManager;
		m_ShipManager = NULL;
	}
	for (Element::Element_List_it_t it = m_lElements.begin(); it != m_lElements.end(); it++)
		delete (*it);
}

void AAMonitorImpl::Init()
{
	if (m_pAADX == NULL)
	{
		m_pAADX = new AADX(m_hWnd);
		m_pAADX->Init3D(m_hWnd);
    }
	_time64(&m_CurTime);
}

void AAMonitorImpl::OnUpdateData(AANotifyMsg *msg)
{
	if (m_ShipManager)
		m_ShipManager->UpdataShip(msg, m_Centor, m_Radius, m_Angle);
}

void AAMonitorImpl::OnRcvFusionPara(AANotifyMsg *msg)
{
	SetShipCount(msg->m_FusionPara.m_ShipCount);
	SetRadarRange(msg->m_FusionPara.m_RadarRange);
	m_ShipManager = new AAShipManager(m_pAADX, m_ShipCount, m_RadarRange);
	m_ShipManager->SetNotifyWin(m_hWnd);
}

void AAMonitorImpl::ShowMonitor()
{
	//S64 now;
	//_time64(&now);
	//if (m_CurTime + m_Timespan > now)
	//{
	//	::Sleep(500);
	//	return;
	//}
	//m_CurTime = now;
	::Sleep(100);
	if (m_pAADX == NULL)
		return;

	m_pAADX->BeginRender();
	if (m_ShipManager != NULL)
		m_ShipManager->DrawShip();
	for (Element::Element_List_it_t it = m_lElements.begin(); it != m_lElements.end(); it++)
		(*it)->Draw();
	for (S32 i = 0; i < m_lControls.GetSize(); i++)
		m_lControls.GetAt(i)->Render();
	m_pAADX->EndRender();
}

Bool AAMonitorImpl::AddCircle(const POINT &center, const U32 r, const U32 color, const char *dscp)
{
	Element *circle = new AACircle(center, r, color, dscp, m_pAADX);
	m_lElements.push_back(circle);
	return TRUE;
}

Bool AAMonitorImpl::AddRect(const RECT &rc, const U32 color, const char *dscp)
{
	Element *rect = new AARect(rc, color, dscp, m_pAADX);
	m_lElements.push_back(rect);
	return TRUE;
}

Bool AAMonitorImpl::AddText(AATEXTINFO &textinfo, const char *title)
{
	Element *text = new AAText(textinfo, title, m_pAADX);
	m_lElements.push_back(text);
	return TRUE;
}

Bool AAMonitorImpl::AddLine(const POINT &start, const POINT &end, U32 color, const char *dscp)
{
	Element *line = new AALine(start, end, color, dscp, m_pAADX);
	m_lElements.push_back(line);
	return TRUE;
}

Bool AAMonitorImpl::AddDial(Element *element, U32 color, F32 angle)
{
	Element *dial = new AADial(element, color, m_pAADX, angle);
	m_lElements.push_back(dial);
	return TRUE;
}

Bool AAMonitorImpl::AddStatic(AAMonitorImpl::AATEXTINFO &textinfo, S32 id)
{
	AAGUIStatic *gui = new AAGUIStatic(textinfo, m_pAADX, id);
	//gui->SetID(id);
	m_lControls.Insert(id, gui);
	return TRUE;
}

void AAMonitorImpl::SetGUIText(S32 id, TCHAR *text)
{
	AAGUIControl *gui = m_lControls.GetAt(id);
	gui->SetUserData(text);
}

Bool AAMonitorImpl::AddButton(AAMonitorImpl::AATEXTINFO &textinfo, S32 id)
{
	AAGUIButton *button = new AAGUIButton(textinfo, m_pAADX, id);
	//button->SetID(id);
	m_lControls.Insert(id, button);
	return TRUE;
}

Bool AAMonitorImpl::HandleMouse(U32 Msg, WPARAM wparam, LPARAM lparam)
{
	Bool handled = FALSE;
	if (this->m_ShipManager)
		handled = m_ShipManager->HandleMouse(Msg, wparam, lparam);
	if (!handled)
	{
		for (S32 i = 0; i < m_lControls.GetSize() /*&& !handled*/; i++)
			handled = m_lControls.GetAt(i)->HandleMouse(Msg, wparam, lparam);
	}
	return TRUE;
}

void AAMonitorImpl::SetEleAngle(F32 angle)
{
	Element::Element_List_it_t it;
	for (it = m_lElements.begin(); it != m_lElements.end(); it++)
		(*it)->SetAngle(angle);
	this->m_Angle = angle;
}