/*
*	@file: AAFusion.cpp
*	@brief: implementation of AIS and radar data fusion
*	@author: dongzhaoyu
*	@date: 2008-5-11
*
*	Copyright (C) CTEC Xi'an JiaoTong University
*/

#include "AAFusion.h"
#include <math.h>
#include <iomanip>

//-------------------------------------------------------------------------------------------------------------
//implementation of AAFusion
//-------------------------------------------------------------------------------------------------------------
AAFusion::AAFusion() : AAReader()
{
	m_FusionThread = NULL;
	InitializeCriticalSection(&(this->m_CS));

#if defined _DEBUG
	using namespace std;
	m_debugfile.open("debug_out.txt", ios_base::out);
	m_debugfile<<setprecision(20);
#endif
}

AAFusion::AAFusion(S32 shipcount) : AAReader(shipcount)
{
	m_FusionThread = NULL;
	InitializeCriticalSection(&(this->m_CS));

#if defined _DEBUG
	using namespace std;
	m_debugfile.open("debug_out.txt", ios_base::out);
	m_debugfile<<setprecision(20);
#endif
}

AAFusion::AAFusion(S32 shipcount, SceneVisitor *ais, SceneVisitor *radar, FusionProc fusionproc) : AAReader(shipcount)
{
	m_ShipCount = shipcount;
	InitResult();

	m_AISSampler = ais;
	m_RadarSampler = radar;

	m_Work = TRUE;
	m_FusionProc = fusionproc;
	InitializeCriticalSection(&(this->m_CS));

	DWORD threadid;
	m_FusionThread = CreateThread(NULL, 0, m_FusionProc, (void*)this, 0, &threadid);

#if defined _DEBUG
	using namespace std;
	m_debugfile.open("debug_out.txt", ios_base::out);
	m_debugfile<<setprecision(20);
#endif
}

AAFusion::~AAFusion()
{
	if (m_FusionThread != NULL)
	{
		m_Work = FALSE;
		if (WaitForSingleObject(m_FusionThread, INFINITE) == WAIT_OBJECT_0)
			CloseHandle(m_FusionThread);
	}

	m_FusionThread = NULL;
	m_FusionProc = NULL;
	m_AISSampler = NULL;
	m_RadarSampler = NULL;
	DeleteCriticalSection(&(this->m_CS));

#if defined _DEBUG
	m_debugfile.close();
#endif
}

void AAFusion::InitResult()
{
	FusionPara para;
	AISData data;
	MODELPARA mpara;
	ZeroMemory(&mpara, sizeof(mpara));
	ZeroMemory(&para, sizeof(para));
	ZeroMemory(&data, sizeof(data));
	CGrowableArray<FusionPara> array;
	for (S32 j = 0; j < m_ShipCount; j++)
		array.Add(para);
	for (S32 i = 0; i < m_ShipCount; i++)
		m_Result.Add(array);
	for (S32 k = 0; k < m_ShipCount; k++)
	{
		m_AISFusionBuffer.Add(data);
		m_RadarPara.Add(mpara);
	}
}

void AAFusion::CalAB(S32 index, FINNERPARA &para, S32 mark)	// mark = 1 angle mark = 2 speed mark = other distance
{
	if (m_Radar[index].size() > RADARPOINTCOUNT)
	{
		RadarIterator rit = m_Radar[index].begin();
		F64 currsum1 = 0.0f, startvalue = 0.0f;
		ASSIGN_BY_MARK(currsum1, rit, mark);
		F64 currsum2 = currsum1, currmean = 0.0f, meansum = 0.0f;
		F64 totalmul = 0.0f, totalsquare = 0.0f, currvalue = 0.0f;
		startvalue = currsum1;

		for(rit++; rit != m_Radar[index].end(); rit++)
		{
			ASSIGN_BY_MARK(currvalue, rit, mark);
			currsum2 += currvalue;
			currmean = (currsum1 + currsum2) / 2.0f;
			meansum += currmean;
			totalmul += currmean * currvalue;
			totalsquare += currmean * currmean;
			currsum1 = currsum2;
		}
		para.m_c = meansum;
		para.m_d = currsum1 - startvalue;
		para.m_e = totalmul;
		para.m_f = totalsquare;
		para.m_l = (S32)m_Radar[index].size() - 1;

		currvalue = para.m_l * para.m_f - para.m_c * para.m_c;
		if (ABS(currvalue) < 0.00001f)
			currvalue = 0.00001;
		para.m_a = (para.m_c * para.m_d - para.m_l * para.m_e) / currvalue;
		if (ABS(para.m_a) < 1.0e-7)
			para.m_a = 0.0f;
		para.m_b = (para.m_f * para.m_d - para.m_c * para.m_e) / currvalue;

		para.m_l++;
	}
	else
	{
		para.m_l = -1;		//this mean the parameter was illegal
	}
}

void AAFusion::SynArray(StandbySetEle &stele, S32 aindex, SynEle &synele, S32 mark)	// mark = 1 angle mark = 2 speed mark = other distance
{
	static F64 startvalue, x1, x2;
	static S64 starttime;
	static RadarIterator rit;
	static AISIterator ait;
	static S32 sk/*, ssize*/;
	static F64 data;
	static FINNERPARA innerpara;

	static F64 valuealpha, constalpha, preresult, result;
	//static S32 deltatime;

	//rit = m_Radar[rindex].begin();
	rit = stele.m_start;
	ASSIGN_BY_MARK(startvalue, rit, mark);
	ASSIGN_BY_MARK(innerpara, &m_RadarPara[stele.m_index], mark);

	if (innerpara.m_a != 0)
		constalpha = innerpara.m_b / innerpara.m_a;
	else
		constalpha = 0.0f;
	starttime = rit->m_UTC;
	valuealpha = startvalue - constalpha;

	ait = m_AIS[aindex].begin();
	while (ait->m_UTC <= starttime && ait != m_AIS[aindex].end())
	{
		PUSH_BY_MARK(&synele, startvalue, mark);
		ait++;
	}

	sk = 1;
	preresult = startvalue;
	x1 = startvalue;
	x2 = valuealpha * pow((F64)E, (F64)(-innerpara.m_a * sk)) + constalpha;
	result = x2 - x1;
	if (result == 0)
		result = startvalue;

	while (ait != m_AIS[aindex].end())
	{
		while (ait->m_UTC >= starttime + RCYCLE)
		{
			sk++;
			preresult = result;
			x1 = x2;
			x2 = valuealpha * pow((F64)E, (F64)(-innerpara.m_a * sk)) + constalpha;
			result = x2 - x1;
			if (result == 0)
				result = startvalue;
			starttime += RCYCLE;
		}

		if (ait->m_UTC != starttime)
			data = preresult + (result - preresult) * (ait->m_UTC - starttime) * ARCYCLE;
		else
			data = preresult;

		PUSH_BY_MARK(&synele, data, mark);
		ait++;

	}
}

void AAFusion::SetFusionResult(S32 rindex, S32 aindex, FusionPara &para)
{
	AALock(&(this->m_CS));

	m_Result[rindex][aindex].m_PointCount += para.m_PointCount;
	m_Result[rindex][aindex].m_RelationDegree += para.m_RelationDegree;

	assert(m_AIS[aindex].empty() == FALSE);
	AISIterator ait = m_AIS[aindex].end();
	ait--;									//we needn't to check if ait is legal, because m_AIS[aindex] is not empty
	m_AISFusionBuffer[aindex] = *ait;		//copy the latest ais data to the fusion buffer
}

Bool AAFusion::GetFusionResult(S32 rindex, AISData &data)
{
	AALock(&(this->m_CS));

	ZeroMemory(&data, sizeof(data));
	F64 result = F64_MIN, temp = 0.0f;
	S32 aindex = 0;
	for (S32 i = 0; i < this->m_ShipCount; i++)
	{
		if (m_Result[rindex][i].m_PointCount > 0)
		{
			temp = m_Result[rindex][i].m_RelationDegree/* / m_Result[rindex][i].m_PointCount*/;
			if (temp > result)
			{
				result = temp;
				aindex = i;
			}
		}
	}

	if (result != F64_MIN)
	{
		data = m_AISFusionBuffer[aindex];
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

S32 AAFusion::CalRelationDegree(S32 aindex, AASynArray &synarray)	//calculate the grey relation degree
{
	static AISIterator ait;
	static F64 minangle, maxangle, minspeed, maxspeed,
		mindistance, maxdistance, totalrd, currd, temp1, temp2, currdegree;
	static S32 rmatch;
	static AASynArray::iterator it;
	static AAF64List::iterator fit;
	static FusionPara fusionpara;

	totalrd = F64_MIN;
	rmatch = 0;
	minangle = minspeed = mindistance = F64_MAX;
	maxangle = maxspeed = maxdistance = F64_MIN;

	for (it = synarray.begin(); it != synarray.end(); it++)
	{

//a macro to preprocess the samples and calculate the minv and maxv
#define GETMAXMIN(maxv, minv, var1, var2) do {\
		assert(m_AIS[aindex].size() == it##->##var1.size());\
		ait = m_AIS[aindex].begin();\
		for (fit = it##->##var1.begin(); fit != it##->##var1.end(); fit++, ait++)\
		{\
			*fit = ABS(*fit - ait##->##var2);\
			minv = MIN(*fit, minv);\
			maxv = MAX(*fit, maxv);\
		}\
}while(0)

		GETMAXMIN(maxangle, minangle, m_Angle, m_Head);
		GETMAXMIN(maxspeed, minspeed, m_Speed, m_Speed);
		GETMAXMIN(maxdistance, mindistance, m_Distance, m_Distance);
	}

	for (it = synarray.begin(); it != synarray.end(); it++)
	{
		currd = 0.0f;

//a macro to calculate the grey relation degree
#define CALDEGREE(maxv, minv, var1) do {\
		temp1 = ROW * maxv;\
		temp2 = minv + temp1;\
		ait = m_AIS[aindex].begin();\
		for (fit = it##->##var1.begin(); fit != it##->##var1.end(); fit++, ait++)\
		{\
			if (*fit == -temp1)\
				currdegree = 1;\
			else\
				currdegree = temp2 / (*fit + temp1);\
			currd += currdegree;\
		}\
}while(0)

		CALDEGREE(maxangle, minangle, m_Angle);
		CALDEGREE(maxspeed, minspeed, m_Speed);
		CALDEGREE(maxdistance, mindistance, m_Distance);

		if (currd > totalrd)
		{
			totalrd = currd;
			rmatch = it->m_rindex;
		}
	}

	fusionpara.m_PointCount = (S32)m_AIS[aindex].size();
	fusionpara.m_RelationDegree = totalrd;
	SetFusionResult(rmatch, aindex, fusionpara);
	return rmatch;
}

void AAFusion::FusionData()
{
	static S32 rindex, aindex;
	static S64 starttime, diff;
	//static S32 aiscycle;
	//static RadarIterator rit;
	static AISIterator ait;
	//static F64 tempspeed = 0.0f, tempangle = 0.0f, tempdistance = 0.0f;
	//static F64 tempspeed0, tempangle0, tempdistance0;

	//fusion para
	static MODELPARA modelpara;
	static AAStandbySet prematchset;
	static StandbySetEle standbyele;
	static SynEle synele;
	static AASynArray synarray;

	while (m_Work)
	{
		::Sleep(1000);	//wait for one seconde

		//read data from generator
		m_AISSampler->AcceptReader(this);
		m_RadarSampler->AcceptReader(this);

		//the implementation of the algorithm
		//fist step: calculate paras for each radar trace
		for (rindex = 0; rindex < m_ShipCount; rindex++)
		{
			CalAngleAB(rindex, modelpara.m_Angle);
			CalSpeedAB(rindex, modelpara.m_Speed);
			CalDistanceAB(rindex, modelpara.m_Distance);
			m_RadarPara.SetAt(rindex, modelpara);
		}

		for (aindex = 0; aindex < m_Size && !m_AIS[aindex].empty(); aindex++)
		{
			if (m_AIS[aindex].size() > AISPOINTCOUNT)	//a ship runs out of the range leaving only a few samples less than RADARPOINTCOUNT
			{
				m_AIS[aindex].clear();
				m_Radar[aindex].clear();
				continue;
			}

			//second step: calculate standbyset for each ais sampled trace
			prematchset.clear();
			ait = m_AIS[aindex].begin();
			starttime = ait->m_UTC;
			for (rindex = 0; rindex < m_Size/* && m_Radar[rindex].size() > 4*/; rindex++)
			{
				//search for the start point
				RadarIterator radarstart;
				for (radarstart = m_Radar[rindex].begin(); radarstart != m_Radar[rindex].end(); radarstart++)
				{
					diff = radarstart->m_UTC - starttime;
					if ((diff >= 0 && diff < RCYCLE) || (diff < 0 && diff > -RCYCLE))
						break;
				}
				if (radarstart != m_Radar[rindex].end() &&
					DISTANCE(ait->m_Latitude, ait->m_Longitude, radarstart->m_Latitude, radarstart->m_Longitude) <= RCYCLE * radarstart->m_Speed + RADARERROR + AISERROR)
				{
					standbyele.m_index = rindex;
					standbyele.m_start = radarstart;
					prematchset.push_back(standbyele);
				}
			}

			//third step: synchronize the time stamp of radar samples by paras
			if (!prematchset.empty())
			{
				synarray.clear();
				for (AAStandbySet::iterator sit = prematchset.begin();
					sit != prematchset.end() && m_Radar[sit->m_index].size() > RADARPOINTCOUNT;
					sit++)
				{
					SynAngleArray(*sit, aindex, synele);
					SynSpeedArray(*sit, aindex, synele);
					SynDistanceArray(*sit, aindex, synele);
					synele.m_rindex = sit->m_index;
					synarray.push_back(synele);
				}

				//forth step: calculate the grey relation degree
				if (!synarray.empty())
				{
					rindex = CalRelationDegree(aindex, synarray);

#if defined _DEBUG
					this->show();
					this->ShowStandbySet(aindex, prematchset);
					this->ShowSynArray(aindex, synarray);
					this->ShowResult();
#endif

					//lastly delete the ais samples and radar samples
					m_Radar[rindex].clear();
					m_AIS[aindex].clear();
				}

			}
			else		//the ship sampled by ais is out of range, so delete
			{
				m_AIS[aindex].clear();
				m_Radar[aindex].clear();	//clear the radar samples for safe
			}
		}
	}
}

#if defined _DEBUG
void AAFusion::show()
{

#define of m_debugfile

	S32 i;
//#if (FALSE)
//	of<<"=============================AIS从数据产生模块采样的数据============================"<<std::endl;
//	for (i = 0; i < m_Size; i++)
//	{
//		of<<"****************---------第"<<i<<"条船------------******************"<<std::endl;
//		for (AISDataElement::iterator it = m_AIS[i].begin(); it != m_AIS[i].end(); it++)
//		{
//			
#define SHOWAIS(stream, it) do {\
			stream<<"呼号：\t"<<it->m_CallSign<<std::endl;\
			stream<<"目标：\t"<<it->m_Dest<<std::endl;\
			stream<<"到达时间：\t"<<it->m_ETA<<std::endl;\
			stream<<"船首向：\t"<<it->m_Head<<std::endl;\
			stream<<"x坐标：\t"<<it->m_Latitude<<std::endl;\
			stream<<"船长度：\t"<<it->m_Length<<std::endl;\
			stream<<"y坐标：\t"<<it->m_Longitude<<std::endl;\
			stream<<"MMSI：\t"<<it->m_MMSI<<std::endl;\
			stream<<"船名：\t"<<it->m_Name<<std::endl;\
			stream<<"编号：\t"<<it->m_ShipID<<std::endl;\
			stream<<"速度：\t"<<it->m_Speed<<std::endl;\
			stream<<"状态：\t"<<it->m_Status<<std::endl;\
			stream<<"时间：\t"<<it->m_UTC<<std::endl;\
			stream<<"船宽：\t"<<it->m_Width<<std::endl;\
			stream<<"距离：\t"<<it->m_Distance<<std::endl;\
			stream<<"--------------------完----------------------"<<std::endl;\
}while(0)
//		
//			SHOWAIS(of, it);
//		}
//		m_AIS[i].clear();
//	}
//#endif

	of<<"=============================Radar从数据产生模块采样的数据============================"<<std::endl;
	for (i = 0; i < m_Size && m_Radar[i].size() > RADARPOINTCOUNT; i++)
	{
		of<<"****************---------第"<<i<<"条船------------******************"<<std::endl;
		for (RadarDataElement::iterator it = m_Radar[i].begin(); it != m_Radar[i].end(); it++)
		{

#define SHOWRADAR(stream, it) do {\
			stream<<"编号:\t"<<it->m_ShipID<<std::endl;\
			stream<<"AIS周期:\t"<<it->m_AISCycle<<std::endl;\
			stream<<"角度:\t"<<it->m_Angle<<std::endl;\
			stream<<"距离:\t"<<it->m_Distance<<std::endl;\
			stream<<"速度:\t"<<it->m_Speed<<std::endl;\
			stream<<"时间:\t"<<it->m_UTC<<std::endl;\
			stream<<"x坐标:\t"<<it->m_Latitude<<std::endl;\
			stream<<"y坐标:\t"<<it->m_Longitude<<std::endl;\
			stream<<"--------------------完----------------------"<<std::endl;\
}while(0)

			SHOWRADAR(of, it);
		}

#define SHOWPARA(str, var) of<<str<<var<<std::endl

		of<<"----------------------------------模型参数------------------------------"<<std::endl;
		of<<"------速度模型参数------"<<std::endl;
		SHOWPARA("a = ", m_RadarPara[i].m_Speed.m_a);
		SHOWPARA("b = ", m_RadarPara[i].m_Speed.m_b);
		SHOWPARA("c = ", m_RadarPara[i].m_Speed.m_c);
		SHOWPARA("d = ", m_RadarPara[i].m_Speed.m_d);
		SHOWPARA("e = ", m_RadarPara[i].m_Speed.m_e);
		SHOWPARA("f = ", m_RadarPara[i].m_Speed.m_f);
		SHOWPARA("l = ", m_RadarPara[i].m_Speed.m_l);
		of<<"------距离模型参数--------"<<std::endl;
		SHOWPARA("a = ", m_RadarPara[i].m_Distance.m_a);
		SHOWPARA("b = ", m_RadarPara[i].m_Distance.m_b);
		SHOWPARA("c = ", m_RadarPara[i].m_Distance.m_c);
		SHOWPARA("d = ", m_RadarPara[i].m_Distance.m_d);
		SHOWPARA("e = ", m_RadarPara[i].m_Distance.m_e);
		SHOWPARA("f = ", m_RadarPara[i].m_Distance.m_f);
		SHOWPARA("l = ", m_RadarPara[i].m_Distance.m_l);
		of<<"------角度模型参数---------"<<std::endl;
		SHOWPARA("a = ", m_RadarPara[i].m_Angle.m_a);
		SHOWPARA("b = ", m_RadarPara[i].m_Angle.m_b);
		SHOWPARA("c = ", m_RadarPara[i].m_Angle.m_c);
		SHOWPARA("d = ", m_RadarPara[i].m_Angle.m_d);
		SHOWPARA("e = ", m_RadarPara[i].m_Angle.m_e);
		SHOWPARA("f = ", m_RadarPara[i].m_Angle.m_f);
		SHOWPARA("l = ", m_RadarPara[i].m_Angle.m_l);
		of<<"-------------------------------模型参数完---------------------------------"<<std::endl;

		//m_Radar[i].clear();
	}
}

void AAFusion::ShowStandbySet(S32 aindex, AAStandbySet &set)
{
	of<<"------------------AIS采样的第"<<aindex<<"条船---------------------------"<<std::endl;
	for (AISIterator ait = m_AIS[aindex].begin(); ait != m_AIS[aindex].end(); ait++)
	{
		SHOWAIS(of, ait);
	}

	of<<"--------预匹配集合---------"<<std::endl;
	for (AAStandbySet::iterator sit = set.begin(); sit != set.end(); sit++)
	{
		of<<"radar_index = "<<sit->m_index<<std::endl;
		of<<"start_time = "<<(sit->m_start)->m_UTC<<std::endl;
		of<<"-------------------------------"<<std::endl;
	}
	//m_AIS[aindex].clear();
}

void AAFusion::ShowSynArray(S32 aindex, AASynArray &array)
{
	of<<"------------------AIS采样的第"<<aindex<<"条船---------------------------"<<std::endl;
	//for (AISIterator ait = m_AIS[aindex].begin(); ait != m_AIS[aindex].end(); ait++)
	//{
	//	SHOWAIS(of, ait);
	//}

	of<<"----------同步后的数据-------------"<<std::endl;
	for (AASynArray::iterator arrayit = array.begin(); arrayit != array.end(); arrayit++)
	{
		of<<"------与雷达中第"<<arrayit->m_rindex<<"条船同步的结果--------"<<std::endl;
		of<<"速度: ";
		for (AAF64List::iterator fit = arrayit->m_Speed.begin(); fit != arrayit->m_Speed.end(); fit++)
			of<<"\t"<<*fit;
		of<<"\n角度: ";
		for (fit = arrayit->m_Angle.begin(); fit != arrayit->m_Angle.end(); fit++)
			of<<"\t"<<*fit;
		of<<"\n距离: ";
		for (fit = arrayit->m_Distance.begin(); fit != arrayit->m_Distance.end(); fit++)
			of<<"\t"<<*fit;
		of<<std::endl;
	}
	of<<"----------同步数据完--------"<<std::endl;
	//m_AIS[aindex].clear();
}

void AAFusion::ShowResult()
{
	static S32 k;
	of<<"-------------------------数据匹配结果"<<k++<<"-----------------------"<<std::endl;
	for (S32 i = 0; i < m_ShipCount; i++)
	{
		of<<"---------雷达第"<<i<<"条船---------"<<std::endl;
		for (S32 j = 0; j < m_ShipCount; j++)
		{
			of<</*"point_count = "<<*/m_Result[i][j].m_PointCount;
			of<</*" && relation_degree = "*/" "<<m_Result[i][j].m_RelationDegree<<"\t";
		}
		of<<std::endl;
	}
}
#endif

//--------------------------------------------------------------------------------------------------------------------
//implementation of fusion procedure
//--------------------------------------------------------------------------------------------------------------------
DWORD WINAPI AAFusionData(LPVOID para)
{
	AAFusion *fusion = (AAFusion*)para;
	fusion->FusionData();
	return 0;
}