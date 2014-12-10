/*
*	@file: AARFSView.h
*	@brief: interface of the CAARFSView class
*	@author: dongzhaoyu
*	@date: 2008-4-14
*
*	Copyright (C) CTEC Xi'an JiaoTong University
*/

#include ".\AAMonitor\AAMonitorImpl.h"
#include ".\AACommon\AANotifyMessage.h"

#pragma once
#pragma warning (disable: 4098)

#define GUI_STATIC_TEST			0

//AIS data items
#define GUI_STATIC_SHIPNAME		(GUI_STATIC_TEST + 1)
#define GUI_STATIC_CALLSIGN		(GUI_STATIC_SHIPNAME + 1)
#define GUI_STATIC_MMSI			(GUI_STATIC_CALLSIGN + 1)
#define GUI_STATIC_LATLONG		(GUI_STATIC_MMSI + 1)
#define GUI_STATIC_STATUS		(GUI_STATIC_LATLONG + 1)
#define GUI_STATIC_DEST			(GUI_STATIC_STATUS + 1)
#define GUI_STATIC_ETA			(GUI_STATIC_DEST + 1)
#define GUI_STATIC_TYPE			(GUI_STATIC_ETA + 1)
#define GUI_STATIC_SPEEDDIR		(GUI_STATIC_TYPE + 1)
#define GUI_STATIC_SIZE			(GUI_STATIC_SPEEDDIR + 1)
#define GUI_STATIC_RANGE		(GUI_STATIC_SIZE + 1)
#define GUI_STATIC_RECEIVED		(GUI_STATIC_RANGE + 1)

//ship data item
#define GUI_STATIC_VELOCITY		(GUI_STATIC_RECEIVED + 1)
#define GUI_STATIC_DIR			(GUI_STATIC_VELOCITY + 1)
#define GUI_STATIC_LONG			(GUI_STATIC_DIR + 1)
#define GUI_STATIC_LAT			(GUI_STATIC_LONG + 1)

//radar para
#define GUI_STATIC_ALARM		(GUI_STATIC_LAT + 1)
#define GUI_STATIC_MOVEMODE		(GUI_STATIC_ALARM + 1)
#define GUI_STATIC_EBL1			(GUI_STATIC_MOVEMODE + 1)
#define GUI_STATIC_VRM1			(GUI_STATIC_EBL1 + 1)
#define GUI_STATIC_EBL2			(GUI_STATIC_VRM1 + 1)
#define GUI_STATIC_VRM2			(GUI_STATIC_EBL2 + 1)
#define GUI_STATIC_TRIAL		(GUI_STATIC_VRM2 + 1)
#define GUI_STATIC_TUNE			(GUI_STATIC_TRIAL + 1)
#define GUI_STATIC_HL			(GUI_STATIC_TUNE + 1)

//tools
#define GUI_BUTTON_AZ			(GUI_STATIC_HL + 1)
#define GUI_BUTTON_PI			(GUI_BUTTON_AZ + 1)
#define GUI_BUTTON_ARPA			(GUI_BUTTON_PI + 1)
#define GUI_BUTTON_SYSTEM		(GUI_BUTTON_ARPA + 1)
#define GUI_BUTTON_NAV			(GUI_BUTTON_SYSTEM + 1)
#define GUI_BUTTON_MAPS			(GUI_BUTTON_NAV + 1)

class CAARFSView : public CWindowImpl<CAARFSView>
{
public:
#define WND_STYLE (CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS)
	DECLARE_WND_CLASS_EX(NULL, WND_STYLE, (HBRUSH)GetStockObject(BLACK_BRUSH))

	HWND m_MainWin;
	AAMonitorImpl m_AAMonitor;
	RECT m_Range;
	HWND m_FusionWin;
	S32 m_Angle;

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(CAARFSView)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLbuttonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLbuttonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_GUIEVENT, OnGUIEvent)
		MESSAGE_HANDLER(WM_AANOTIFYMESSAGE, OnNotifyMsg)
		MESSAGE_HANDLER(WM_AASHIPSELECTED, OnShipSelected)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	void Init()
	{
		m_Angle = 0;
		m_AAMonitor.SethWnd((HWND)(*this));
		m_AAMonitor.SetTimespan(1);
		m_AAMonitor.Init();

		::GetClientRect((HWND)(*this), &m_Range);
		U8 off1 = 10, off2 = 10;
		m_Range.top += off1;
		m_Range.bottom -= off2;
		m_Range.left += off1;
		m_Range.right -= off2;
		m_AAMonitor.AddRect(m_Range, 0x00ff0000, "my first rect");

		POINT s, e;
		U32 lineoffset = 200;
		s.x = e.x = m_Range.right - lineoffset;
		s.y = m_Range.top;
		e.y = m_Range.bottom;
		m_AAMonitor.AddLine(s, e, 0x00ff0000, "my first line");

		POINT p;
		U32 ry = m_Range.bottom - m_Range.top;
		U32 rx = m_Range.right - m_Range.left - lineoffset;
		ry = ry >> 1;
		rx = rx >> 1;
		p.x = m_Range.left + rx;
		p.y = m_Range.top + ry;
		U32 r = min(rx, ry) - 50;
		m_AAMonitor.AddDial(new AACircle(p, r, 0x00ccbb00, "center", m_AAMonitor.GetpAADX()), 0xffff0000, 0);
		m_AAMonitor.SetCentor(p);
		m_AAMonitor.SetRadius(r);
		m_AAMonitor.SetAngle(0);
		r -= 50;
		while(r >= 50)
		{
			m_AAMonitor.AddCircle(p, r, 0x00ff0000, "my first circle");
			r -= 50;
		}

		U32 staticoffset = 0;
		AAMonitorImpl::AATEXTINFO textinfo;
		StringCchCopy(textinfo.backfile, FILE_PATH_LEN, _T("")/*_T(".\\res\\redscal.bmp")*/);
		textinfo.color = 0x00ff0000;
		textinfo.fontname = _T("Arial");
		textinfo.height = 20;
		textinfo.italic = FALSE;
		textinfo.str = _T("AIS DATA");
		textinfo.weight = 0;
		textinfo.width = 10;

		staticoffset = (lineoffset - 8 * textinfo.width) >> 1;
		textinfo.x = m_Range.right - lineoffset + staticoffset;

		textinfo.y = m_Range.top + 10;
		textinfo.format = DT_CENTER | DT_VCENTER;
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_TEST);

		//textinfo.y = m_Range.bottom - 30;
		//textinfo.str = _T("button");
		//StringCchCopy(textinfo.backfile, FILE_PATH_LEN, _T(".\\res\\button.bmp"));
		//m_AAMonitor.AddButton(textinfo, GUI_BUTTON_TEST);

		StringCchCopy(textinfo.backfile, FILE_PATH_LEN, _T("")/*_T(".\\res\\redscal.bmp")*/);
		textinfo.height = 12;
		textinfo.width = 6;
		textinfo.x = m_Range.right - lineoffset + 5;
		textinfo.y = m_Range.top + 34;
		textinfo.str = _T("NAME:      ");
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_SHIPNAME);

		textinfo.y += (4 + textinfo.height);
		textinfo.str = _T("CALLSIGN:  ");
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_CALLSIGN);

		textinfo.y += (4 + textinfo.height);
		textinfo.str = _T("MMSI:      ");
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_MMSI);

		textinfo.y += (4 + textinfo.height);
		textinfo.str = _T("Lat/Long:       ");
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_LATLONG);

		textinfo.y += (4 + textinfo.height);
		textinfo.str = _T("STATUS:    ");
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_STATUS);

		textinfo.y += (4 + textinfo.height);
		textinfo.str = _T("DEST:      ");
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_DEST);

		textinfo.y += (4 + textinfo.height);
		textinfo.str = _T("ETA:       ");
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_ETA);

		textinfo.y += (4 + textinfo.height);
		textinfo.str = _T("TYPE:      ");
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_TYPE);

		textinfo.y += (4 + textinfo.height);
		textinfo.str = _T("SPEED/DIR: ");
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_SPEEDDIR);

		textinfo.y += (4 + textinfo.height);
		textinfo.str = _T("SIZE:      ");
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_SIZE);

		textinfo.y += (4 + textinfo.height);
		textinfo.str = _T("RANGE:     ");
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_RANGE);

		textinfo.y += (4 + textinfo.height);
		textinfo.str = _T("TIME:  ");
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_RECEIVED);

		POINT a, b;
		a.x = m_Range.right - lineoffset;
		a.y = textinfo.y + textinfo.height + 4;
		b.x = m_Range.right;
		b.y = a.y;
		m_AAMonitor.AddLine(a, b, 0xffff0000, "");

		textinfo.y = a.y + 4;
		textinfo.height = 20;
		textinfo.width = 10;
		staticoffset = (lineoffset - 9 * textinfo.width) >> 1;
		textinfo.x = m_Range.right - lineoffset + staticoffset - 10;
		textinfo.str = _T("SHIP DATA");
		m_AAMonitor.AddText(textinfo, "");

		textinfo.x = m_Range.right - lineoffset + 5;
		textinfo.y += 24;
		textinfo.height = 12;
		textinfo.width = 6;
		textinfo.str = _T("VELOCITY:  ");
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_VELOCITY);

		textinfo.y += (4 + textinfo.height);
		textinfo.str = _T("DIR:       ");
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_DIR);

		textinfo.y += (4 + textinfo.height);
		textinfo.str = _T("LONG:      ");
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_LONG);

		textinfo.y += (4 + textinfo.height);
		textinfo.str = _T("LAT:       ");
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_LAT);

		a.y = b.y = textinfo.y + textinfo.height + 4;
		m_AAMonitor.AddLine(a, b, 0xffff0000, "");

		textinfo.y += (14 + textinfo.height);
		textinfo.height = 20;
		textinfo.width = 10;
		textinfo.str = _T("RADAR PARA");
		staticoffset = (lineoffset - 10 * textinfo.width) >> 1;
		textinfo.x = m_Range.right - lineoffset + staticoffset - 10;
		m_AAMonitor.AddText(textinfo, "");

		textinfo.y += (textinfo.height + 4);
		textinfo.x = m_Range.right - lineoffset + 5;
		textinfo.height = 12;
		textinfo.width = 6;
		textinfo.str = _T("ALARM:     NO");
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_ALARM);

		textinfo.y += (textinfo.height + 4);
		textinfo.str = _T("MOVEMODE:  RM");
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_MOVEMODE);

		textinfo.y += (textinfo.height + 4);
		textinfo.str = _T("EBL1:      OFF");
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_EBL1);

		textinfo.y += (textinfo.height + 4);
		textinfo.str = _T("VRM1:      OFF");
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_VRM1);

		textinfo.y += (textinfo.height + 4);
		textinfo.str = _T("EBL2:      OFF");
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_EBL2);

		textinfo.y += (textinfo.height + 4);
		textinfo.str = _T("VRM2:      OFF");
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_VRM2);

		textinfo.y += (textinfo.height + 4);
		textinfo.str = _T("TRIAL:     OFF");
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_TRIAL);

		textinfo.y += (textinfo.height + 4);
		textinfo.str = _T("TUNE:      ON");
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_TUNE);
	
		textinfo.y += (textinfo.height + 4);
		textinfo.str = _T("HL:        OFF");
		m_AAMonitor.AddStatic(textinfo, GUI_STATIC_HL);

		a.y = b.y = textinfo.y + textinfo.height + 4;
		m_AAMonitor.AddLine(a, b, 0xffff0000, "");

		textinfo.height = 20;
		textinfo.width = 10;
		textinfo.str = _T("    AZ    ");
		StringCchCopy(textinfo.backfile, FILE_PATH_LEN, _T(".\\res\\button.bmp"));
		textinfo.x = m_Range.right - lineoffset + 10;
		textinfo.y = a.y + 24;
		m_AAMonitor.AddButton(textinfo, GUI_BUTTON_AZ);

		textinfo.x += 10 * textinfo.width + 8;
		textinfo.str = _T("    PI    ");
		m_AAMonitor.AddButton(textinfo, GUI_BUTTON_PI);

		textinfo.x = m_Range.right - lineoffset + 10;
		textinfo.y += 54;
		textinfo.str = _T(" ARPA  ");
		m_AAMonitor.AddButton(textinfo, GUI_BUTTON_ARPA);

		textinfo.x += 10 * textinfo.width + 8;
		textinfo.str = _T("  SYS  ");
		m_AAMonitor.AddButton(textinfo, GUI_BUTTON_SYSTEM);

		textinfo.x = m_Range.right - lineoffset + 10;
		textinfo.y += 54;
		textinfo.str = _T("   NAV   ");
		m_AAMonitor.AddButton(textinfo, GUI_BUTTON_NAV);

		textinfo.x += 10 * textinfo.width + 8;
		textinfo.str = _T(" MAPS ");
		m_AAMonitor.AddButton(textinfo, GUI_BUTTON_MAPS);

		textinfo.height = 20;
		textinfo.width = 10;
		textinfo.x = m_Range.left + 10;
		textinfo.y = m_Range.top + 10;
		StringCchCopy(textinfo.backfile, FILE_PATH_LEN, _T("")/*_T(".\\res\\redscal.bmp")*/);
		textinfo.str = _T("RANGE  0.25NM");
		m_AAMonitor.AddText(textinfo, "");

		textinfo.y += 24;
		textinfo.str = _T("RINGS ON");
		m_AAMonitor.AddText(textinfo, "");

		textinfo.y += 24;
		textinfo.str = _T("HEAD UP");
		m_AAMonitor.AddText(textinfo, "");

		textinfo.x = m_Range.right - lineoffset - 150;
		textinfo.y = m_Range.top + 10;
		textinfo.format = DT_RIGHT;
		textinfo.str = _T("WIND  DEPTH");
		m_AAMonitor.AddText(textinfo, "");

		textinfo.y += 24;
		textinfo.str = _T("SPEED  0.0KT");
		m_AAMonitor.AddText(textinfo, "");

		textinfo.y = m_Range.bottom - 24 * 3 - 10;
		textinfo.x = m_Range.left + 10;
		textinfo.str = _T("ENH  OFF");
		m_AAMonitor.AddText(textinfo, "");

		textinfo.y += 24;
		textinfo.str = _T("RAIN MAN");
		m_AAMonitor.AddText(textinfo, "");

		textinfo.y += 24;
		textinfo.str = _T("SEA  MAN");
		m_AAMonitor.AddText(textinfo, "");

		textinfo.x = m_Range.right - lineoffset - 150;
		textinfo.str = _T("NIGHT MODE");
		m_AAMonitor.AddText(textinfo, "");
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		m_AAMonitor.ShowMonitor();

		SetMsgHandled(false);
		return 1;
	}

	LRESULT OnLbuttonDown(UINT msg, WPARAM wPara, LPARAM lPara, BOOL& bHandled)
	{
		m_AAMonitor.HandleMouse(msg, wPara, lPara);
		SetMsgHandled(false);
		return 0;
	}

	LRESULT OnMouseMove(UINT msg, WPARAM wPara, LPARAM lPara, BOOL &bHandled)
	{
		m_AAMonitor.HandleMouse(msg, wPara, lPara);
		SetMsgHandled(false);
		return 0;
	}

	LRESULT OnLbuttonUp(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		m_AAMonitor.HandleMouse(msg, wParam, lParam);
		SetMsgHandled(false);
		return 0;
	}

	LRESULT OnGUIEvent(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		S32 step;
		TCHAR value[50];
		switch(wParam)
		{
		case GUI_BUTTON_AZ:
			if (m_Angle != 0)
			{
				step = m_Angle <= 180 ? -1 : 1;
				m_Angle += step;
				if (m_Angle == 360)
					m_Angle = 0;
				m_AAMonitor.SetEleAngle(m_Angle);
				_stprintf(value, _T("DIR:       %d.00"), m_Angle);
				m_AAMonitor.SetGUIText(GUI_STATIC_DIR, value);
			}
			if (m_Angle != 0)
				PostMessage(WM_GUIEVENT, GUI_BUTTON_AZ, 0);
			break;
		case GUI_BUTTON_PI:
			this->SetFocus();
			break;
		}
		return 0;
	}

	LRESULT OnNotifyMsg(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		TCHAR value[100];
		AANotifyMsg *notifymsg = reinterpret_cast<AANotifyMsg*>(wParam);
		if (notifymsg->m_Type == MsgT_NotifyShipPos)
		{
			m_AAMonitor.OnUpdateData(notifymsg);
		}
		else if (notifymsg->m_Type == MsgT_NotifyRadarPos)
		{
			m_AAMonitor.SetGUIText(GUI_STATIC_VELOCITY, _T("VELOCITY:  0"));
			m_AAMonitor.SetGUIText(GUI_STATIC_DIR, _T("DIR:       0 "));

			_stprintf(value, _T("LONG:      %f"), notifymsg->m_RadarPos.m_Longitude);
			m_AAMonitor.SetGUIText(GUI_STATIC_LONG, value);

			_stprintf(value, _T("LAT:       %f"), notifymsg->m_RadarPos.m_Latitude);
			m_AAMonitor.SetGUIText(GUI_STATIC_LAT, value);
		}
		else if (notifymsg->m_Type == MsgT_NotifyFusionPara)
		{
			m_AAMonitor.OnRcvFusionPara(notifymsg);
		}
		else if (notifymsg->m_Type == MsgT_NotifyFusionResult)
		{
			_stprintf(value, _T("NAME:   %s"), notifymsg->m_FusionResult.m_Name);
			m_AAMonitor.SetGUIText(GUI_STATIC_SHIPNAME, value);

			_stprintf(value, _T("CALLSIGN:   %s"), notifymsg->m_FusionResult.m_CallSign);
			m_AAMonitor.SetGUIText(GUI_STATIC_CALLSIGN, value);

			_stprintf(value, _T("MMSI:   %s"), notifymsg->m_FusionResult.m_MMSI);
			m_AAMonitor.SetGUIText(GUI_STATIC_MMSI, value);

			_stprintf(value, _T("Lat/Long:   %s"), notifymsg->m_FusionResult.m_LatLong);
			m_AAMonitor.SetGUIText(GUI_STATIC_LATLONG, value);

			_stprintf(value, _T("STATUS:   %s"), notifymsg->m_FusionResult.m_Status);
			m_AAMonitor.SetGUIText(GUI_STATIC_STATUS, value);

			_stprintf(value, _T("DEST:   %s"), notifymsg->m_FusionResult.m_Dest);
			m_AAMonitor.SetGUIText(GUI_STATIC_DEST, value);

			_stprintf(value, _T("ETA:   %s"), notifymsg->m_FusionResult.m_ETA);
			m_AAMonitor.SetGUIText(GUI_STATIC_ETA, value);

			_stprintf(value, _T("TYPE:   %s"), notifymsg->m_FusionResult.m_Type);
			m_AAMonitor.SetGUIText(GUI_STATIC_TYPE, value);

			_stprintf(value, _T("SPEED/DIR:   %s"), notifymsg->m_FusionResult.m_SpeedDir);
			m_AAMonitor.SetGUIText(GUI_STATIC_SPEEDDIR, value);

			_stprintf(value, _T("SIZE:   %s"), notifymsg->m_FusionResult.m_Size);
			m_AAMonitor.SetGUIText(GUI_STATIC_SIZE, value);

			_stprintf(value, _T("RANGE:   %s"), notifymsg->m_FusionResult.m_Range);
			m_AAMonitor.SetGUIText(GUI_STATIC_RANGE, value);

			_stprintf(value, _T("TIME:   %s"), notifymsg->m_FusionResult.m_Received);
			m_AAMonitor.SetGUIText(GUI_STATIC_RECEIVED, value);
		}
		delete notifymsg;
		return 0;
	}

	LRESULT OnShipSelected(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		::PostMessage(m_FusionWin, WM_AACALLFORFUSIONRESULT, wParam, lParam);
		return 0;
	}

	LRESULT OnKeyDown(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
	{
		TCHAR value[50];
		switch(wParam)
		{
		case VK_RIGHT:
			m_Angle += 1;
			if (m_Angle >= 360)
				m_Angle -= 360;
			m_AAMonitor.SetEleAngle((F32)m_Angle);
			_stprintf(value, _T("DIR:       %d"), m_Angle);
			m_AAMonitor.SetGUIText(GUI_STATIC_DIR, value);
			break;
		case VK_LEFT:
			m_Angle -= 1;
			if (m_Angle < 0)
				m_Angle += 360;
			m_AAMonitor.SetEleAngle((F32)m_Angle);
			_stprintf(value, _T("DIR:       %d"), m_Angle);
			m_AAMonitor.SetGUIText(GUI_STATIC_DIR, value);
			break;
		default:
			break;
		}
		SetMsgHandled(false);
		return 0;
	}
};
