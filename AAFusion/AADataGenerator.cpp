/*
*	@file: AADataGenerator.cpp
*	@brief: the implementation of AADataGenerator.h
*	@author: dongzhaoyu
*	@date: 2008-5-7
*	
*	Copyright (C) CTEC Xi'an JiaoTong University
*/

#include "AADataGenerator.h"
#include "..\AACommon\AANotifyMessage.h"
#include <strsafe.h>
#include <math.h>
#include <iostream>

//----------------------------------------------------------------------------------------------------------
//the implementation of help function
//----------------------------------------------------------------------------------------------------------
//the Gauss-Kruger Projection Coordinate is as below
//  /\ x
//	|
//	| (topx, lefty)			(topx, righty)
//	|	+----------------------+
//	|	|					   |
//	|	|					   |
//	|	+----------------------+
//	| (bottomx, lefty)      (bottomx, righty)
//	|
//	|(0, 0)								y
//	+--------------------------------->

typedef void (__stdcall * GKProcedure)(F64, F64, F64&, F64&, F64, F64, S32, S32);

void GKProjection(F64 central, F64 longitude, F64 latitude, F64 &x, F64 &y)
{
	HINSTANCE hdll = LoadLibrary(_T(".\\AALibrary\\GSPROJ.dll"));
	GKProcedure gkp = NULL;
	if (hdll)
	{
		gkp = (GKProcedure)GetProcAddress(hdll, "GS");
		gkp(latitude, longitude, y, x, 0, central, 3, 6);
		FreeLibrary(hdll);
	}
	else
	{
		MessageBox(NULL, _T("can not find GSPROJ.dll"), _T("ERROR"), MB_OK);
		exit(0);
	}
}

void GKProjectionInv(F64 central, F64 x, F64 y, F64 &longitude, F64 &latitude)
{
	HINSTANCE hdll = LoadLibrary(_T(".\\AALibrary\\GSPROJ.dll"));
	GKProcedure gkp = NULL;
	if (hdll)
	{
		gkp = (GKProcedure)GetProcAddress(hdll, "GSinv");
		gkp(y, x, latitude, longitude, 0, central, 3, 6);
		FreeLibrary(hdll);
	}
	else
	{
		MessageBox(NULL, _T("can not find GSPROJ.dll"), _T("ERROR"), MB_OK);
		exit(0);
	}
}

//-----------------------------------------------------------------------------------------------------------
//the function for generat angle speed and time period
//-----------------------------------------------------------------------------------------------------------
S32 AARAND(S32 a, S32 b, S32 offset)
{
	assert(b > a);
	static Bool init = FALSE;
	if (!init)
	{
		srand((U32)time(NULL) + offset);
		init = TRUE;
	}
	return (rand() % (b - a)) + a;
}

F64 AAAngle(F64 angle, F64 maxanglechange)
{
	F64 delta = ((F64)AARAND(0, 1999 << 1) - 1999) / (F64)1999.0f;
	F64 rand = delta * maxanglechange + angle;
	while (rand >= 360)
		rand -= 360;
	while (rand < 0)
		rand += 360;
	return rand;
}

F64 AASpeed(F64 speed, F64 mins, F64 maxs, F64 maxchange)
{
	S32 retry = 10;
	F64 delta, rand, change;
	//static S32 offset;
	//offset++;
	do
	{
		delta = ((F64)AARAND(0, 1999 << 1/*, offset % 100*/) - 1999) / (F64)1999.0;
		change = maxchange * delta;
		rand = speed + change;
		retry--;
	}while((rand < mins || rand > maxs) && retry > 0);
	if (rand < mins)
		rand = mins + abs(change);
	else if (rand > maxs)
		rand = maxs - abs(change);
	return rand;
}

//------------------------------------------------------------------------------------------------------------
//the implementation of RadarSampler
//------------------------------------------------------------------------------------------------------------
RadarSampler::RadarSampler() : SceneVisitor()
{
	this->m_MaxSize = 0;
	this->m_Posx = 0.0f;
	this->m_Posy = 0.0f;
	this->m_Radius = 0.0f;
	this->m_Cycle = 3;
}

RadarSampler::RadarSampler(F64 x, F64 y, F64 radius, HWND notifywin) : SceneVisitor(x, y),
													m_Cycle(3), m_Radius(radius), m_hNotifyWin(notifywin)
{
	m_MaxSize = 9999;
}

RadarSampler::~RadarSampler()
{}

void RadarSampler::VisitScene(AADataGenerator *scene)
{
	AALock(&(this->m_CS));
	S32 shipcount = scene->GetShipCount();
	ShipInfo *ship = NULL;
	S64 time = scene->GetTime();
	RadarData sample;
	F64 distance;
	AANotifyMsg *msg = NULL;

	if (m_Buffer.size() < m_MaxSize)
	{
		for (S32 index = 0; index < shipcount; index++)
		{
			ship = scene->GetShip(index);
			distance = DISTANCE(m_Posx, m_Posy, ship->m_Latitude, ship->m_Longitude);
			if ((m_Radius > distance) && (ship->m_LastRadarSampleTime + m_Cycle <= time))
			{
				ship->m_LastRadarSampleTime = time;
				sample.m_ShipID = ship->m_ShipID;
				sample.m_Angle = ship->m_Head;
				sample.m_Distance = distance;
				sample.m_AISCycle = ship->m_AISCycle;
				sample.m_Speed = ship->m_MoveSpeed;
				sample.m_UTC = ship->m_UTC;
				sample.m_Latitude = ship->m_Latitude;
				sample.m_Longitude = ship->m_Longitude;
				m_Buffer.push_back(sample);

				//here we must notify the view to update the ship's position
				msg = new AANotifyMsg;
				msg->m_Type = MsgT_NotifyShipPos;
				msg->m_ShipPos.m_Angle = GetLocationAngle(sample.m_Latitude, sample.m_Longitude);
				msg->m_ShipPos.m_Distance = sample.m_Distance;
				msg->m_ShipPos.m_ShipID = sample.m_ShipID;
				::PostMessage(this->m_hNotifyWin, WM_AANOTIFYMESSAGE, reinterpret_cast<U32>(msg), 0);
			}
		}
	}
}

F64 RadarSampler::GetLocationAngle(F64 x, F64 y)
{
	F64 cx = x - m_Posx;
	F64 cy = y - m_Posy;

	if (abs(F64(cy)) <= 0.0001)
	{
		if (cx > 0)
			return 3.1415 * 0.5f;
		else
			return -3.1415 * 0.5f;
	}
	if (cy > 0)
		return atan((F64)(cx / cy));
	else
		return 3.1415926 + atan(F64(cx / cy));
}

void RadarSampler::AcceptReader(AAReader *reader)
{
	AALock(&(this->m_CS));
	reader->VisitRadar(this);
	m_Buffer.clear();
}

//------------------------------------------------------------------------------------------------------------
//the implementation of AISSampler
//------------------------------------------------------------------------------------------------------------
AISSampler::AISSampler(F64 x, F64 y) : SceneVisitor(x, y)
{
	m_MaxSize = 9999;
}

AISSampler::~AISSampler()
{}

void AISSampler::VisitScene(AADataGenerator *scene)
{
	AALock(&(this->m_CS));
	S32 shipcount = scene->GetShipCount();
	AISData sampler;
	S64 time = scene->GetTime();
	ShipInfo *ship = NULL;

	if (m_Buffer.size() < m_MaxSize)
	{
		for (S32 index = 0; index < shipcount; index++)
		{
			ship = scene->GetShip(index);
			if (ship->m_LastAISSampleTime + ship->m_AISCycle <= time)
			{
				ship->m_LastAISSampleTime = time;
				ship->m_AISCycle = AISCycle(*ship);

				sampler.m_ShipID = ship->m_ShipID;
				StringCchCopy(sampler.m_CallSign, CALLSIGN_LEN + 3, ship->m_CallSign);
				StringCchCopy(sampler.m_Dest, DEST_LEN + 2, ship->m_Dest);
				sampler.m_ETA = ship->m_ETA;
				sampler.m_Head = ship->m_Head;
				sampler.m_Latitude = ship->m_Latitude;
				sampler.m_Length = ship->m_Length;
				sampler.m_Longitude = ship->m_Longitude;
				StringCchCopy(sampler.m_MMSI, MMSI_LEN + 3, ship->m_MMSI);
				StringCchCopy(sampler.m_Name, NAME_LEN + 2, ship->m_Name);
				sampler.m_Speed = ship->m_MoveSpeed;
				StringCchCopy(sampler.m_Status, STATUS_LEN + 2, ship->m_Status);
				sampler.m_UTC = ship->m_UTC;
				sampler.m_Width = ship->m_Width;
				sampler.m_Distance = DISTANCE(ship->m_Latitude, ship->m_Longitude, m_Posx, m_Posy);
				StringCchCopy(sampler.m_Type, TYPE_LEN, ship->m_Type);

				m_Buffer.push_back(sampler);
			}
		}
	}
}

void AISSampler::AcceptReader(AAReader *reader)
{
	AALock(&(this->m_CS));
	reader->VisitAIS(this);
	m_Buffer.clear();
}

S32 AISSampler::AISCycle(const ShipInfo &ship)
{
	F64 delta = (ship.m_MoveSpeed - ship.m_MinSpeed) / (ship.m_MaxSpeed - ship.m_MinSpeed);
	S32 cycle;
	switch (ship.m_SimState)
	{
	case SIMULATION_FORWARD:
		if (delta <= 0.1f)
			cycle = 12;
		else if (delta <= 0.3f)
			cycle = 6;
		else if (delta <= 0.5f)
			cycle = 4;
		else if (delta <= 0.8f)
			cycle = 3;
		else
			cycle = 2;
		break;
	case SIMULATION_LEFT_ROTATE:
	case SIMULATION_RIGHT_ROTATE:
		break;
	default:
		break;
	}
	return cycle;
}

//------------------------------------------------------------------------------------------------------------
//the implementation of AAReader
//------------------------------------------------------------------------------------------------------------
AAReader::AAReader()
{
	m_Size = 0;
}

AAReader::AAReader(S32 size)
{
	m_Size = size;
	this->m_Radar.SetSize(m_Size);
	this->m_AIS.SetSize(m_Size);
	S32 i = 0;
	RadarDataElement radar;
	AISDataElement ais;
	for (i = 0; i < m_Size; i++)
	{
		m_Radar.Add(radar);
		m_AIS.Add(ais);
	}
}

void AAReader::VisitAIS(AISSampler *sampler)
{
	const AISSampler::AISBuffer *buffer = sampler->GetBuffer();
	DataCopy(m_AIS, buffer);
}

void AAReader::VisitRadar(RadarSampler *sampler)
{
	const RadarSampler::RadarBuffer *buffer = sampler->GetBuffer();
	DataCopy(m_Radar, buffer);
}

//------------------------------------------------------------------------------------------------------------
//the implementation of AADataGenerator
//------------------------------------------------------------------------------------------------------------
AADataGenerator::AADataGenerator()
{
	ZeroMemory(&(this->m_SceneInfo), sizeof(m_SceneInfo));
	m_pShip = NULL;
	m_WorkThread = NULL;
	//m_Event = NULL;
	m_pRadar = NULL;
	m_pAIS = NULL;
	m_UpdateProc = NULL;
}

AADataGenerator::AADataGenerator(SceneInfo &info, /*HANDLE event,*/ SceneVisitor *radar, SceneVisitor *ais, UpdateProc updateproc)
{
	CopyMemory(&(this->m_SceneInfo), &info, sizeof(SceneInfo));

	F64 topx, bottomx, lefty, righty;
	GKProjection(m_SceneInfo.m_CentralLongitude, m_SceneInfo.m_LeftLongitude, m_SceneInfo.m_BottomLatitude, bottomx, lefty);
	GKProjection(m_SceneInfo.m_CentralLongitude, m_SceneInfo.m_RightLongitude, m_SceneInfo.m_TopLatitude, topx, righty);
	m_LeftCoordinate = lefty;
	m_BottomCoordinate = bottomx;
	m_SceneLength = righty - lefty;
	m_SceneWidth = topx - bottomx;

	m_pShip = new ShipInfo[m_SceneInfo.m_ShipCount];
	_time64(&m_Time);
	m_TimeStep = 1;

	//m_Event = event;
	//assert(m_Event != NULL);
	m_pRadar = radar;
	m_pAIS = ais;
	m_VisitorCycle = 2;
	m_LastVisitorTime = m_Time - 1000;

	InitShip();

	m_UpdateProc = updateproc;
	DWORD threadid;
	m_Work = TRUE;
	m_WorkThread = CreateThread(NULL, 0, m_UpdateProc, (void*)this, 0, &threadid);
}

AADataGenerator::~AADataGenerator()
{
	if (m_WorkThread != NULL)
	{
		m_Work = FALSE;							//notify the work thread to terminate
		if (WaitForSingleObject(m_WorkThread, INFINITE) == WAIT_OBJECT_0)
		{
			CloseHandle(m_WorkThread);
		}
	}
	delete[] m_pShip;
	m_pShip = NULL;
	//m_Event = NULL;
	m_pRadar = NULL;
	m_pAIS = NULL;
	m_UpdateProc = NULL;
}

void AADataGenerator::InitShip()
{
	S32 shipindex = 0;
	ShipInfo *ship = NULL;
	for (shipindex = 0; shipindex < m_SceneInfo.m_ShipCount; shipindex++)
	{
		ship = m_pShip + shipindex;
		ship->m_ShipID = shipindex;

		_stprintf(ship->m_Name, _T("Test_Ship_%-3d"), shipindex);
		_stprintf(ship->m_CallSign, _T("%0.5d"), shipindex);
		_stprintf(ship->m_MMSI, _T("%0.9d"), shipindex + 11);

		ship->m_Length = (U32)AARAND(10, 100, shipindex << 6);
		ship->m_Width = (U32)AARAND(5, 20, shipindex << 6);
		ship->m_Latitude = m_BottomCoordinate + (F64)AARAND(0, (S32)m_SceneWidth, shipindex << 6);
		ship->m_Longitude = m_LeftCoordinate + (F64)AARAND(0, (S32)m_SceneLength, shipindex << 6);

		ship->m_MaxSpeed = 18.5f;
		ship->m_MinSpeed = 0.0f;
		ship->m_MaxSpeedChange = 2.5f;
		ship->m_MaxTimePeriod = 20;
		ship->m_MinTimePeriod = 3;
		ship->m_MaxAngleChange = 2.5f;
		ship->m_MaxRotationSpeed = 10.5f;
		ship->m_MinRotationSpeed = 0.0f;

		ship->m_MoveSpeed = (ship->m_MinSpeed + ship->m_MaxSpeed) / 2.0f;
		ship->m_RotationSpeed = (ship->m_MinRotationSpeed + ship->m_MaxRotationSpeed) / 2.0f;
		ship->m_Head = (F64)AARAND(0, 359, shipindex << 6) / 2.0f;
		ship->m_RemainTime = AARAND(ship->m_MinTimePeriod, ship->m_MaxTimePeriod, shipindex << 6);

		StringCchCopy(ship->m_Status, STATUS_LEN, _T("MOVE"));
		StringCchCopy(ship->m_Dest, DEST_LEN, _T("HongKong"));
		ship->m_UTC = _time64(NULL);
		ship->m_ETA = ship->m_UTC + 24 * 60 * 60;
		ship->m_SimState = SIMULATION_FORWARD;
		ship->m_AISCycle = AISSampler::AISCycle(*ship);
		ship->m_LastAISSampleTime = ship->m_UTC - 1000;
		ship->m_LastRadarSampleTime = ship->m_UTC - 1000;
		StringCchCopy(ship->m_Type, TYPE_LEN, _T("Fishing"));
	}
}

void AADataGenerator::Update()
{
	static S32 shipindex;
	static const F64 delta = 3.1415926 / (F64)180;
	static F64 alpha, range;
	static ShipInfo *ship;
	while (m_Work)
	{
#if defined _DEBUG
		std::cout<<std::endl<<"Update is running!"<<std::endl;
#endif
		m_Time += m_TimeStep;
		for (shipindex = 0; shipindex < m_SceneInfo.m_ShipCount; shipindex++)
		{
			ship = m_pShip + shipindex;
			alpha = ship->m_Head * delta;
			range = ship->m_MoveSpeed * m_TimeStep;
			switch (ship->m_SimState)
			{
			case SIMULATION_FORWARD:
				ship->m_Latitude += cos(alpha) * range;
				ship->m_Longitude += sin(alpha) * range;
				ship->m_RemainTime -= m_TimeStep;
				ship->m_UTC = m_Time;
				if (ship->m_RemainTime <= 0)
				{
					ship->m_RemainTime = AARAND((S32)(ship->m_MinTimePeriod), (S32)(ship->m_MaxTimePeriod));
					ship->m_MaxAngleChange = ship->m_Head * 0.1f;
					ship->m_MaxSpeedChange = ship->m_MoveSpeed * 0.1f;
					ship->m_Head = AAAngle(ship->m_Head, ship->m_MaxAngleChange);
					ship->m_MoveSpeed = AASpeed(ship->m_MoveSpeed, ship->m_MinSpeed, ship->m_MaxSpeed, ship->m_MaxSpeedChange);
					ship->m_AISCycle = AISSampler::AISCycle(*ship);
				}
				break;
			case SIMULATION_LEFT_ROTATE:
				break;
			case SIMULATION_RIGHT_ROTATE:
				break;
			default:
				break;
			}
		}
		if (m_LastVisitorTime + m_VisitorCycle <= m_Time)
		{
			this->Accept(m_pAIS);
			this->Accept(m_pRadar);
		}
		::Sleep(500);
	}
}

void AADataGenerator::Accept(SceneVisitor *visitor)
{
	visitor->VisitScene(this);
}

//------------------------------------------------------------------------------------------------------------------------
//implementation of update thread procedure
//------------------------------------------------------------------------------------------------------------------------
DWORD WINAPI UpdateScene(LPVOID lppara)
{
	AADataGenerator *scene = (AADataGenerator*)lppara;
	scene->Update();
	return 0;
}