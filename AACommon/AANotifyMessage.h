/*
*	@file: AANotifyMessage.h
*	@brief: define the message format to notify the data update
*	@author: dongzhaoyu
*	@date: 2008-6-1
*
*	Copyright (C) CTEC Xi'an JiaoTong University
*/

#ifndef _AANOTIFYMESSAGE_H
#define _AANOTIFYMESSAGE_H

#include <windows.h>
#include "AAStdtype.h"
#pragma warning (disable : 4005)
#include <tchar.h>
#pragma warning (default : 4005)

#define WM_AANOTIFYMESSAGE (WM_USER + 205)
#define WM_AASHIPSELECTED (WM_AANOTIFYMESSAGE + 1)
#define WM_AAGETFUSIONRESULT (WM_AASHIPSELECTED + 1)
#define WM_AACALLFORFUSIONRESULT (WM_AAGETFUSIONRESULT + 1)
#define MSG_ITEM_LEN 50

typedef enum _AAMsgType{
	MsgT_NotifyShipPos,			//a message sended by model to the view to update ship position
	MsgT_NotifyRadarPos,		//a message sended by view to the model to update radar position
	MsgT_NotifyFusionPara,		//a message sended by model to the view to notify the radar range and ship count
	MsgT_NotifyFusionResult,	//a message sended by model to the view to notify the fusion result
}AAMsgType;

typedef struct _AANotifyMsg
{
	AAMsgType m_Type;
	union
	{
		struct NotifyShipPos	//when m_Type = MsgT_NotifyShipPos
		{
			S32 m_ShipID;		//the identifier of the ship
			F64 m_Angle;		//the angle the ship is located
			F64 m_Distance;		//the distance between the ship and radar
		} m_ShipPos;
		struct NotifyRadarPos	//when m_Type = MsgT_NotifyRadarPos
		{
			F64 m_Latitude;		//the latitude
			F64 m_Longitude;	//the longitude
		} m_RadarPos;
		struct NotifyFusionPara	//when m_Type = MsgT_NotifyFusionPara
		{
			F64 m_RadarRange;	//the range of the radar
			S32 m_ShipCount;	//total ship count in the simulation
		} m_FusionPara;
		struct FusionResult		//when m_Type = MsgT_NotifyFusionResult
		{
			TCHAR m_Name[MSG_ITEM_LEN];
			TCHAR m_CallSign[MSG_ITEM_LEN];
			TCHAR m_MMSI[MSG_ITEM_LEN];
			TCHAR m_LatLong[MSG_ITEM_LEN];
			TCHAR m_Status[MSG_ITEM_LEN];
			TCHAR m_Dest[MSG_ITEM_LEN];
			TCHAR m_ETA[MSG_ITEM_LEN];
			TCHAR m_Type[MSG_ITEM_LEN];
			TCHAR m_SpeedDir[MSG_ITEM_LEN];
			TCHAR m_Size[MSG_ITEM_LEN];
			TCHAR m_Range[MSG_ITEM_LEN];
			TCHAR m_Received[MSG_ITEM_LEN];
		} m_FusionResult;
	};
}AANotifyMsg;

#endif