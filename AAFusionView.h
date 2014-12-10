/*
*	@file: AAFusionView.h
*	@brief: implementation of controls on fusion panel
*	@author: dongzhaoyu
*	@date: 2008-5-30
*
*	Copyright(C) CTEC Xi'an JiaoTong University
*/

#if _MSC_VER > 1200
#pragma once
#endif

#include "stdafx.h"
#include "./AAFusion/AAFusion.h"
#include "./AACommon/AANotifyMessage.h"
#include <time.h>

#define OKBUTTON 5000

class AAFusionView : public CWindowImpl<AAFusionView>
{
public:
#define WND_STYLE (CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS)
	DECLARE_WND_CLASS_EX(NULL, WND_STYLE, (HBRUSH)GetStockObject(WHITE_BRUSH))

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(AAFusionView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_AACALLFORFUSIONRESULT, OnCallForFusionResult)
		COMMAND_HANDLER(OKBUTTON, BN_CLICKED, OnOk)
	END_MSG_MAP()

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		RECT rc;
		int height = 20;
		int lap = height >> 1;
		int left, top, length;
		length = 140;
		top = 20;
		left = 8;

		rc.left = left;
		rc.top = top;
		rc.right = length - left;
		rc.bottom = top + height;
		m_Title.Create(this->m_hWnd, &rc, _T("GK_Parameters"), WS_CHILD | WS_VISIBLE);

		rc.left = left;
		rc.top = (top += height + lap);
		rc.right = length - left;
		rc.bottom = top + height;
		m_CentralLogitude.Create(this->m_hWnd, &rc, _T("Central_Longitude:"), WS_CHILD | WS_VISIBLE);

		rc.left = left;
		rc.top = (top += height + lap);
		rc.right = length - left;
		rc.bottom = top + height;
		m_CLEdit.Create(this->m_hWnd, &rc, _T("21"), WS_CHILD | WS_VISIBLE | WS_BORDER);

		rc.left = left;
		rc.top = (top += height + lap);
		rc.right = length - left;
		rc.bottom = top + height;
		m_LeftLogitude.Create(this->m_hWnd, &rc, _T("Left_Longitude:"), WS_CHILD | WS_VISIBLE);

		rc.left = left;
		rc.top = (top += height + lap);
		rc.right = length - left;
		rc.bottom = top + height;
		m_LLEdit.Create(this->m_hWnd, &rc, _T("20.975"), WS_CHILD | WS_VISIBLE | WS_BORDER);

		rc.left = left;
		rc.top = (top += height + lap);
		rc.right = length - left;
		rc.bottom = top + height;
		m_RightLogitude.Create(this->m_hWnd, &rc, _T("Right_Longitude:"), WS_CHILD | WS_VISIBLE);

		rc.left = left;
		rc.top = (top += height + lap);
		rc.right = length - left;
		rc.bottom = top + height;
		m_RLEdit.Create(this->m_hWnd, &rc, _T("21.025"), WS_CHILD | WS_VISIBLE | WS_BORDER);

		rc.left = left;
		rc.top = (top += height + lap);
		rc.right = length - left;
		rc.bottom = top + height;
		m_BottomLatitude.Create(this->m_hWnd, &rc, _T("Bottom_Latitude:"), WS_CHILD | WS_VISIBLE);

		rc.left = left;
		rc.top = (top += height + lap);
		rc.right = length - left;
		rc.bottom = top + height;
		m_BLEdit.Create(this->m_hWnd, &rc, _T("30.000"), WS_CHILD | WS_VISIBLE | WS_BORDER);

		rc.left = left;
		rc.top = (top += height + lap);
		rc.right = length - left;
		rc.bottom = top + height;
		m_TopLatitude.Create(this->m_hWnd, &rc, _T("Top_Latitude:"), WS_CHILD | WS_VISIBLE);

		rc.left = left;
		rc.top = (top += height + lap);
		rc.right = length - left;
		rc.bottom = top + height;
		m_TLEdit.Create(this->m_hWnd, &rc, _T("30.050"), WS_CHILD | WS_VISIBLE | WS_BORDER);

		rc.left = left;
		rc.top = (top += height + lap);
		rc.right = length - left;
		rc.bottom = top + height;
		m_TotalShip.Create(this->m_hWnd, &rc, _T("Ship_Count:"), WS_CHILD | WS_VISIBLE);

		rc.left = left;
		rc.top = (top += height + lap);
		rc.right = length - left;
		rc.bottom = top + height;
		m_TSEdit.Create(this->m_hWnd, &rc, _T("20"), WS_CHILD | WS_VISIBLE | WS_BORDER);

		rc.left = left;
		rc.top = (top += height + lap);
		rc.right = length - left;
		rc.bottom = top + height;
		m_MaxSpeed.Create(this->m_hWnd, &rc, _T("Max_Speed:"), WS_CHILD | WS_VISIBLE, 0, OKBUTTON);

		rc.left = left;
		rc.top = (top += height + lap);
		rc.right = length - left;
		rc.bottom = top + height;
		m_MaxSEdit.Create(this->m_hWnd, &rc, _T("18.5m/s"), WS_CHILD | WS_VISIBLE | WS_BORDER, 0, OKBUTTON);
		m_MaxSEdit.SetReadOnly();

		rc.left = left;
		rc.top = (top += height + lap);
		rc.right = length - left;
		rc.bottom = top + height;
		m_MinSpeed.Create(this->m_hWnd, &rc, _T("Min_Speed:"), WS_CHILD | WS_VISIBLE, 0, OKBUTTON);

		rc.left = left;
		rc.top = (top += height + lap);
		rc.right = length - left;
		rc.bottom = top + height;
		m_MinSEdit.Create(this->m_hWnd, &rc, _T("0.0m/s"), WS_CHILD | WS_VISIBLE | WS_BORDER, 0, OKBUTTON);
		m_MinSEdit.SetReadOnly();

		rc.left = left;
		rc.top = (top += height + lap);
		rc.right = length - left;
		rc.bottom = top + height;
		m_RadarRange.Create(this->m_hWnd, &rc, _T("Radar_Range:"), WS_CHILD | WS_VISIBLE, 0, OKBUTTON);

		rc.left = left;
		rc.top = (top += height + lap);
		rc.right = length - left;
		rc.bottom = top + height;
		m_RREdit.Create(this->m_hWnd, &rc, _T("4000"), WS_CHILD | WS_VISIBLE | WS_BORDER, 0, OKBUTTON);

		rc.left = left + 20;
		rc.top = (top += height + lap + 10);
		rc.right = length - left - 20;
		rc.bottom = top + height + 10;
		m_OK.Create(this->m_hWnd, &rc, _T("OK"), WS_CHILD | WS_VISIBLE | WS_BORDER, 0, OKBUTTON);

		//initialize the fusion classes
		this->m_pAAFusion = NULL;
		this->m_pAISSampler = NULL;
		this->m_pRadarSampler = NULL;
		this->m_pDataGenerator = NULL;
		ZeroMemory(&m_Scene, sizeof(m_Scene));

		SetMsgHandled(false);
		return 0;
	}

	LRESULT OnOk(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		static bool started = FALSE;
		F64 radarrange;

		if (started == TRUE)
			return 0;
		m_CLEdit.SetReadOnly();
		m_LLEdit.SetReadOnly();
		m_RLEdit.SetReadOnly();
		m_BLEdit.SetReadOnly();
		m_TLEdit.SetReadOnly();
		m_TSEdit.SetReadOnly();
		m_RREdit.SetReadOnly();

		//initialize the scene infomation
		TCHAR value[100];

		m_CLEdit.GetWindowText(value, 99);
		m_Scene.m_CentralLongitude = _tstof(value);

		m_LLEdit.GetWindowText(value, 99);
		m_Scene.m_LeftLongitude = _tstof(value);

		m_RLEdit.GetWindowText(value, 99);
		m_Scene.m_RightLongitude = _tstof(value);

		m_BLEdit.GetWindowText(value, 99);
		m_Scene.m_BottomLatitude = _tstof(value);

		m_TLEdit.GetWindowText(value, 99);
		m_Scene.m_TopLatitude = _tstof(value);

		m_TSEdit.GetWindowText(value, 99);
		m_Scene.m_ShipCount = _tstoi(value);

		m_RREdit.GetWindowText(value, 99);
		radarrange = _tstof(value);

		//initialize the radar ais and generator
		F64 radarx, radary;
		F64 logitude, latitude;
		logitude = m_Scene.m_CentralLongitude;
		latitude = m_Scene.m_BottomLatitude + (m_Scene.m_TopLatitude - m_Scene.m_BottomLatitude) * 0.5f;
		GKProjection(logitude, logitude, latitude, radarx, radary);

		//notify the radar monitor view of the radar position for showing
		AANotifyMsg *msg = new AANotifyMsg;
		msg->m_Type = MsgT_NotifyRadarPos;
		msg->m_RadarPos.m_Latitude = latitude;
		msg->m_RadarPos.m_Longitude = logitude;
		::SendMessage(m_NotifyWin, WM_AANOTIFYMESSAGE, reinterpret_cast<U32>(msg), 0);

		msg = new AANotifyMsg;
		msg->m_Type = MsgT_NotifyFusionPara;
		msg->m_FusionPara.m_ShipCount = m_Scene.m_ShipCount;
		msg->m_FusionPara.m_RadarRange = radarrange;
		::SendMessage(m_NotifyWin, WM_AANOTIFYMESSAGE, reinterpret_cast<U32>(msg), 0);

		m_pRadarSampler = new RadarSampler(radarx, radary, radarrange, m_NotifyWin);
		m_pAISSampler = new AISSampler(radarx, radary);
		m_pDataGenerator = new AADataGenerator(m_Scene, m_pRadarSampler, m_pAISSampler);
		m_pAAFusion = new AAFusion(m_Scene.m_ShipCount, m_pAISSampler, m_pRadarSampler);

		started = TRUE;
		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if (m_pAAFusion)
		{
			delete m_pAAFusion;
			m_pAAFusion = NULL;
		}
		if (m_pDataGenerator)
		{
			delete m_pDataGenerator;
			m_pDataGenerator = NULL;
		}
		if (m_pRadarSampler)
		{
			delete m_pRadarSampler;
			m_pRadarSampler = NULL;
		}
		if (m_pAISSampler)
		{
			delete m_pAISSampler;
			m_pAISSampler = NULL;
		}

		SetMsgHandled(false);
		return 0;
	}

	LRESULT OnCallForFusionResult(UINT Msg, WPARAM wPara, LPARAM lPara, BOOL &bHandled)
	{
		AISData fusiondata;
		if (m_pAAFusion)
		{
			if (m_NotifyWin && m_pAAFusion->GetFusionResult(wPara, fusiondata))
			{
				AANotifyMsg *msg = new AANotifyMsg;
				assert(m_pDataGenerator);
				GKProjectionInv(m_pDataGenerator->GetCentral(), fusiondata.m_Latitude, fusiondata.m_Longitude, fusiondata.m_Longitude, fusiondata.m_Latitude);
				msg->m_Type = MsgT_NotifyFusionResult;
				_stprintf(msg->m_FusionResult.m_Name, _T("%s"), fusiondata.m_Name);
				_stprintf(msg->m_FusionResult.m_CallSign, _T("%s"), fusiondata.m_CallSign);
				_stprintf(msg->m_FusionResult.m_MMSI, _T("%s"), fusiondata.m_MMSI);
				_stprintf(msg->m_FusionResult.m_LatLong, _T("%f / %f"), fusiondata.m_Latitude, fusiondata.m_Longitude);
				_stprintf(msg->m_FusionResult.m_Status, _T("%s"), fusiondata.m_Status);
				_stprintf(msg->m_FusionResult.m_Dest, _T("%s"), fusiondata.m_Dest);
				_stprintf(msg->m_FusionResult.m_ETA, _T("%s"), _tctime64(&(fusiondata.m_ETA)));
				_stprintf(msg->m_FusionResult.m_Type, _T("%s"), fusiondata.m_Type);
				_stprintf(msg->m_FusionResult.m_SpeedDir, _T("%f / %f"), fusiondata.m_Speed, fusiondata.m_Head);
				_stprintf(msg->m_FusionResult.m_Size, _T("%d * %d"), fusiondata.m_Length, fusiondata.m_Width);
				_stprintf(msg->m_FusionResult.m_Range, _T("%f"), fusiondata.m_Distance);
				_stprintf(msg->m_FusionResult.m_Received, _T("%s"), _tctime64(&(fusiondata.m_UTC)));
				::PostMessage(m_NotifyWin, WM_AANOTIFYMESSAGE, reinterpret_cast<U32>(msg), 0);
			}
		}
		return 0;
	}

	void SetNotifyWin(HWND wnd)
	{
		m_NotifyWin = wnd;
	}

private:
	CStatic m_Title;
	CStatic m_CentralLogitude;
	CStatic m_LeftLogitude;
	CStatic m_RightLogitude;
	CStatic m_BottomLatitude;
	CStatic m_TopLatitude;
	CStatic m_TotalShip;
	CStatic m_MaxSpeed;
	CStatic m_MinSpeed;
	CStatic m_RadarRange;

	CEdit m_CLEdit;
	CEdit m_LLEdit;
	CEdit m_RLEdit;
	CEdit m_BLEdit;
	CEdit m_TLEdit;
	CEdit m_TSEdit;
	CEdit m_MaxSEdit;
	CEdit m_MinSEdit;
	CEdit m_RREdit;

	CButton m_OK;

	RECT m_Range;
	HWND m_NotifyWin;

	SceneInfo m_Scene;
	RadarSampler *m_pRadarSampler;
	AISSampler *m_pAISSampler;
	AADataGenerator *m_pDataGenerator;
	AAFusion *m_pAAFusion;
};