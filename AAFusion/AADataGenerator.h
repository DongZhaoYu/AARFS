/*
*	@file: AADataGenerator.h
*	@brief: the data generator for ais and arpa radar, the RWP model is used
*	@author: dongzhaoyu
*	@date: 2008-5-5
*	
*	Copyright (C) CTEC Xi'an JiaoTong University
*/

#if _MSC_VER > 1200
#pragma once
#define _AADATAGENERATOR_H
#endif

#include <windows.h>
#include <math.h>
#include <tchar.h>
#include <assert.h>
#include <time.h>
#include <list>
#include "../AACommon/AAStdtype.h"
#include "../AACommon/AAPredefine.h"

//----------------------------------------------------------------------------------------------------
#define NAME_LEN		30
#define CALLSIGN_LEN	5
#define MMSI_LEN		9
#define STATUS_LEN		20
#define DEST_LEN		20
#define TYPE_LEN		20

#define DISTANCE(x1, y1, x2, y2)	(sqrt(((x2) - (x1)) * ((x2) - (x1)) + ((y2) - (y1)) * ((y2) - (y1))))

//---------------------------------------------------------------------------------------------------------
/*	in the data generation procedure, we need to transform the coordinate from x-y to latitude-longitude
	and the opposite, here we use the gauss-kruger projection, the WGS84 standard is used
*/
void GKProjection(F64 central, F64 longitude, F64 latitude, F64 &x, F64 &y);
void GKProjectionInv(F64 central, F64 x, F64 y, F64 &longitude, F64 &latitude);

//generat a random number between a and b
inline S32 AARAND(S32 a, S32 b, S32 offset = 0);

//generat a random angle
inline F64 AAAngle(F64 angle, F64 maxanglechange);

//generat a random speed
inline F64 AASpeed(F64 speed, F64 mins, F64 maxs, F64 maxchange);

//-----------------------------------------------------------------------------------------------------------
//mark the ship's state in simulation
typedef enum _SIMULATIONSTATE
{
	SIMULATION_FORWARD,					//the ship is normal
	SIMULATION_LEFT_ROTATE,				//the ship stopped and rotating left
	SIMULATION_RIGHT_ROTATE				//the ship stopped and rotationg right
}SIMULATIONSTATE;

//the information for a specified ship
typedef struct _ShipInfo
{
	U32 m_ShipID;

	TCHAR m_Name[NAME_LEN + 2];
	TCHAR m_CallSign[CALLSIGN_LEN + 3];
	TCHAR m_MMSI[MMSI_LEN + 3];			//the MMSI number's length is 9
	U32 m_Length;						//the length of the ship 10 - 100
	U32 m_Width;						//the width of the ship 5 - 10
	F64 m_Latitude;						//the latitude of the ship's location, is equavalent to x coordinate
	F64 m_Longitude;					//the longitude of the ship's location, is equavalent to y coordinate
	F64 m_MoveSpeed;					//speed of the ship
	F64 m_RotationSpeed;				//rotation speed
	F64 m_Head;							//the angle the ship is heading for
	S32 m_RemainTime;					//the remaining time the ship to change state
	TCHAR m_Status[STATUS_LEN + 2];		//ship's status, stop, running or etc.
	TCHAR m_Dest[DEST_LEN + 2];			//the ship's destination
	S64 m_ETA;							//the estimated time to arrival
	S64 m_UTC;							//the universal time coordination
	TCHAR m_Type[TYPE_LEN];				//the type of the ship
	SIMULATIONSTATE m_SimState;			//the ship's state in the simulation

	F64 m_MaxSpeed;						//the maximum speed of the ship
	F64 m_MinSpeed;						//the minimum speed of the ship
	F64 m_MaxSpeedChange;
	S32 m_MaxTimePeriod;				//the maximum time period the ship move at a specified speed
	S32 m_MinTimePeriod;				//the minimum time period the ship move at a specified speed
	F64 m_MaxAngleChange;				//the maximum rotation speed change
	F64 m_MaxRotationSpeed;				//
	F64 m_MinRotationSpeed;				//

	S64 m_LastRadarSampleTime;
	S64 m_LastAISSampleTime;
	S32 m_AISCycle;						//the current cycle of AIS sampler in the ship
}ShipInfo;

//the data can receive from AIS
typedef struct _AISData
{
	S32 m_ShipID;
	TCHAR m_Name[NAME_LEN + 1];			//we assume the maximum length of name is 30
	TCHAR m_CallSign[CALLSIGN_LEN + 1];	//the call-sign length is 5 or 4, add 1 for safe
	TCHAR m_MMSI[MMSI_LEN + 1];			//the MMSI number's length is 9
	U32 m_Length;						//the length of the ship
	U32 m_Width;						//the width of the ship
	F64 m_Latitude;						//the latitude of the ship's location, is equavalent to x coordinate
	F64 m_Longitude;					//the longitude of the ship's location, is equavalent to y coordinate
	F64 m_Head;							//the angle the ship is heading for
	F64 m_Speed;						//speed of the ship
	TCHAR m_Status[STATUS_LEN + 1];		//ship's status, stop, running or etc.
	TCHAR m_Dest[DEST_LEN + 1];			//the ship's destination
	S64 m_ETA;							//the estimated time to arrival
	S64 m_UTC;							//the universal time coordination
	F64 m_Distance;						//the distance of the ship
	TCHAR m_Type[TYPE_LEN];				//the type of the ship
}AISData;

//the data can receive from radar
typedef struct _RadarData
{
	S32 m_ShipID;
	F64 m_Distance;						//the distance of the ship
	F64 m_Angle;						//the angle of the ship
	S32 m_AISCycle;
	F64 m_Speed;
	S64 m_UTC;							//the time the data was received
	F64 m_Latitude;
	F64 m_Longitude;
}RadarData;

//the scene info
typedef struct _SceneInfo
{
	F64 m_LeftLongitude;
	F64 m_RightLongitude;
	F64 m_TopLatitude;
	F64 m_BottomLatitude;
	F64 m_CentralLongitude;

	S32 m_ShipCount;					//the total number of ships
}SceneInfo;

//-------------------------------------------------------------------------------------------------------------
//forwar reference
class RadarSampler;
class AADataGenerator;
class AISSmpler;
class AAReader;

//-------------------------------------------------------------------------------------------------------------
//a class for synchronization
class AALock
{
public:
	inline AALock(CRITICAL_SECTION *cs) { if (cs) EnterCriticalSection(cs); m_pCS = cs; }
	inline ~AALock() { if (m_pCS) LeaveCriticalSection(m_pCS); m_pCS = NULL;}

private:
	CRITICAL_SECTION *m_pCS;
};

//------------------------------------------------------------------------------------------------------------
//the base class for scene visitor
class SceneVisitor
{
public:
	virtual ~SceneVisitor() { DeleteCriticalSection(&m_CS); }
	//virtual Bool NeedUpdate() { AALock(&(this->m_CS)); return m_NeedUpdate; }
	virtual void VisitScene(AADataGenerator*) {};
	//virtual void GetData(void *buffer, U32 &size);
	virtual void AcceptReader(AAReader*) {};

	SET_GET_ACCESSOR(U32, MaxSize)

protected:
	SceneVisitor(F64 x = 0.0f, F64 y = 0.0f) { InitializeCriticalSection(&m_CS); m_MaxSize = 200; m_Posx = x; m_Posy = y; }

	//Bool m_NeedUpdate;
	CRITICAL_SECTION m_CS;
	U32 m_MaxSize;
	F64 m_Posx;							//the ship's position in projection coordinate
	F64 m_Posy;
};

//-------------------------------------------------------------------------------------------------------------
//the radar data sampler, a concrete visitor to scene
class RadarSampler : public SceneVisitor
{
public:
	typedef std::list<RadarData> RadarBuffer;

	RadarSampler();
	RadarSampler(F64 x, F64 y, F64 radius, HWND notifywin = NULL);
	~RadarSampler();

	virtual void VisitScene(AADataGenerator*);
	virtual void AcceptReader(AAReader*);
	virtual const RadarBuffer* GetBuffer() const { return &m_Buffer; }
	inline F64 GetLocationAngle(F64 x, F64 y);	//calculate the location angle of the ship related to radar

	SET_GET_ACCESSOR(U32, Cycle)

private:
	RadarBuffer m_Buffer;
	F64 m_Radius;
	U32 m_Cycle;
	HWND m_hNotifyWin;	//a handle to a window to be notifid when the radar sampler read a data
};

//---------------------------------------------------------------------------------------------------------------------
//the AIS data sampler, a concrete visitor to scene
class AISSampler : public SceneVisitor
{
public:
	typedef std::list<AISData> AISBuffer;

	AISSampler(F64 = 0.0f, F64 = 0.0f);
	~AISSampler();

	virtual void VisitScene(AADataGenerator*);
	virtual void AcceptReader(AAReader*);
	virtual const AISBuffer* GetBuffer() const { return &m_Buffer; }
	static S32 AISCycle(const ShipInfo &ship);

private:
	AISBuffer m_Buffer;
};

//-------------------------------------------------------------------------------------------------------------------------
//read data from AIS and radar
class AAReader
{
public:
	typedef std::list<RadarData> RadarDataElement;
	typedef RadarDataElement::iterator RadarIterator;
	typedef std::list<AISData> AISDataElement;
	typedef AISDataElement::iterator AISIterator;
	typedef CGrowableArray<RadarDataElement > RadarDataBuffer;
	typedef CGrowableArray<AISDataElement > AISDataBuffer;

	AAReader();
	AAReader(S32 size);

	virtual ~AAReader() {}
	virtual void VisitRadar(RadarSampler*);
	virtual void VisitAIS(AISSampler*);

protected:
	template<typename ArrayType, typename ContainerType>
	void DataCopy(ArrayType &dest, const ContainerType *source)
	{
		ContainerType::const_iterator it;
		S32 id;
		for (it = source->begin(); it != source->end(); it++)
		{
			id = it->m_ShipID;
			dest[id].push_back(*it);
		}
	}

	S32 m_Size;
	RadarDataBuffer m_Radar;
	AISDataBuffer m_AIS;
};

//-----------------------------------------------------------------------------------------------------------------------
//procedure for the thread to update the scene
DWORD WINAPI UpdateScene(LPVOID lppara);	

//data generation model, maintain a scene for ship simulation
class AADataGenerator
{
public:
	typedef DWORD (WINAPI *UpdateProc)(LPVOID lppara);

	AADataGenerator();
	AADataGenerator(SceneInfo&, /*HANDLE = NULL,*/ SceneVisitor* = NULL, SceneVisitor* = NULL, UpdateProc = UpdateScene);
	~AADataGenerator();									//this class is not used as a parent class

	void Accept(SceneVisitor*);
	ShipInfo* GetShip(S32 index) { assert(m_pShip); return m_pShip + index; }
	S32 GetShipCount() const { return m_SceneInfo.m_ShipCount; }
	void AddTime(U32 slap)	{ m_Time += slap; }
	void Update();
	void InitShip();

	GET_SET_ACCESSOR(SceneVisitor*, pRadar)
	GET_SET_ACCESSOR(SceneVisitor*, pAIS)
	GET_ACCESSOR(__time64_t, Time)
	F64 GetCentral() { return m_SceneInfo.m_CentralLongitude; }

private:
	volatile Bool m_Work;								//main thread set m_work to notify the work thread to terminate

	SceneInfo m_SceneInfo;
	F64 m_LeftCoordinate;
	F64 m_BottomCoordinate;
	F64 m_SceneLength;
	F64 m_SceneWidth;

	ShipInfo *m_pShip;
	__time64_t m_Time;
	U32 m_TimeStep;

	UpdateProc m_UpdateProc;
	HANDLE m_WorkThread;								//the thread to simulate the scene
	//HANDLE m_Event;									//the event to resume the thread, it is created by the main thread

	U32 m_VisitorCycle;
	S64 m_LastVisitorTime;
	SceneVisitor *m_pRadar;								//the radar sampler, created by the main thread
	SceneVisitor *m_pAIS;								//the AIS sampler, created by the main thread
};
//------------------------------------------------------------------------------------------------------------------------