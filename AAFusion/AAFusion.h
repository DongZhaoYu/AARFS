/*
*	@file: AAFusion.h
*	@brief: the definition of the class for AIS and radar data fusion
*	@author: dongzhaoyu
*	@date: 2008-5-11
*
*	Copyright (C) CTEC Xi'an JiaoTong University
*/

#if _MSC_VER > 1200
#pragma once
#define _AAFUSION_H
#endif

#include "AADataGenerator.h"
#include <iostream>
#include <fstream>

#define _ASSIGN_BY_MARK(var, it, mark) do { if ((mark) == 1) var = (it)##->##m_Angle; \
										  else if ((mark) == 2) var = (it)##->##m_Speed; \
										  else var = (it)##->##m_Distance; }while(0)
#define ASSIGN_BY_MARK(var, it, mark) _ASSIGN_BY_MARK(var, it, mark)
#define _SET_BY_MARK(it, var, mark) do { if ((mark) == 1) (it)##->##m_Angle = var; \
										 else if ((mark) == 2) (it)##->##m_Speed = var; \
										 else (it)##->##m_Distance = var; }while(0)
#define SET_BY_MARK(it, var, mark) _SET_BY_MARK(it, var, mark)
#define _PUSH_BY_MARK(it, var, mark) do { if ((mark) == 1) (it)##->##m_Angle.push_back(var); \
										 else if ((mark) == 2) (it)##->##m_Speed.push_back(var); \
										 else (it)##->##m_Distance.push_back(var); }while(0)
#define PUSH_BY_MARK(it, var, mark) _PUSH_BY_MARK(it, var, mark)
#define E 2.71828f
#define RCYCLE 3
#define AISERROR	800
#define RADARERROR	800
#define ARCYCLE (F64)1.0f / (F64)RCYCLE
#define ABS(x) ((x) >= 0 ? (x) : -(x))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define ROW 0.5f
#define RADARPOINTCOUNT 4
#define AISPOINTCOUNT 20

//a procedure execute by the fusion thread, the para will be a pointer to AAFusion Object
DWORD WINAPI AAFusionData(LPVOID);

//AAFusion will fusion the data received by radar and AIS
class AAFusion : public AAReader
{
public:
	typedef DWORD (WINAPI *FusionProc)(LPVOID);
	typedef struct _FusionPara
	{
		S32 m_PointCount;
		F64 m_RelationDegree;
	}FusionPara;					//a struct to store the fusion result
	typedef struct _FINNERPARA
	{
		S32 m_l;
		F64 m_a;
		F64 m_b;
		F64 m_c;
		F64 m_d;
		F64 m_e;
		F64 m_f;
	}FINNERPARA;					//a struct to store the model para
	typedef struct _MODELPARA
	{
		FINNERPARA m_Angle;
		FINNERPARA m_Speed;
		FINNERPARA m_Distance;
	}MODELPARA;						// model para for angle, speed and distance
	typedef struct _StandbySetEle
	{
		S32 m_index;				//the index of radar trace
		RadarIterator m_start;		//the start iterator of Radar trace
	}StandbySetEle;
	typedef std::list<F64> AAF64List;
	typedef struct _SynEle
	{
		AAF64List m_Angle;
		AAF64List m_Speed;
		AAF64List m_Distance;
		S32 m_rindex;
	}SynEle;
	typedef std::list<SynEle> AASynArray;			//a list to store the synchronized samples for a specified ais sampled trace
	typedef std::list<StandbySetEle> AAStandbySet;	//a list to store the standby traces sampled by radar for a specified ais sampled trace
	typedef CGrowableArray<MODELPARA> AARadarPara;	// a array to store the paras for each radar sampled trace
	typedef CGrowableArray<CGrowableArray<FusionPara> > AAFusionResult;	//a 2D array to store the fusion result

	AAFusion();
	AAFusion(S32 shipcount);
	AAFusion(S32 shipcount, SceneVisitor *ais, SceneVisitor *radar, FusionProc fusionproc = AAFusionData);
	~AAFusion();

	void FusionData();
	void SetFusionResult(S32, S32, FusionPara&);	//rindex, aindex
	Bool GetFusionResult(S32, AISData&);			//input rindex, output the associated ais data, return if the data exit or not

#if defined _DEBUG
	void show();
	void ShowStandbySet(S32, AAStandbySet&);
	void ShowSynArray(S32, AASynArray&);
	void ShowResult();
#endif

protected:
	inline void InitResult();
	inline void CalAB(S32, FINNERPARA&, S32);
	void CalAngleAB(S32 index, FINNERPARA &para) { CalAB(index, para, 1); }
	void CalSpeedAB(S32 index, FINNERPARA &para) { CalAB(index, para, 2); }
	void CalDistanceAB(S32 index, FINNERPARA &para) { CalAB(index, para, 3); }
	inline void SynArray(StandbySetEle&, S32, SynEle&, S32);
	void SynAngleArray(StandbySetEle &stele, S32 aindex, SynEle &synele) { synele.m_Angle.clear(); SynArray(stele, aindex, synele, 1); }
	void SynSpeedArray(StandbySetEle &stele, S32 aindex, SynEle &synele) { synele.m_Speed.clear(); SynArray(stele, aindex, synele, 2); }
	void SynDistanceArray(StandbySetEle &stele, S32 aindex, SynEle &synele) { synele.m_Distance.clear(); SynArray(stele, aindex, synele, 3); }
	inline S32 CalRelationDegree(S32, AASynArray&);		//return the associated radar trace index

private:
	volatile Bool m_Work;
	HANDLE m_FusionThread;
	FusionProc m_FusionProc;
	CRITICAL_SECTION m_CS;

	S32 m_ShipCount;
	AAFusionResult m_Result;
	SceneVisitor *m_AISSampler;						//deconstructor can not delete m_AISSampler and m_RadarSampler
	SceneVisitor *m_RadarSampler;

	AARadarPara m_RadarPara;						//the model para for each radar sampled trace
	CGrowableArray<AISData> m_AISFusionBuffer;		//the ais data fused

#if defined _DEBUG
	std::fstream m_debugfile;
#endif
};