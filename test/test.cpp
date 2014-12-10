/*
*	@file: test.cpp
*	@brief: for test
*	@author: dongzhaoyu
*	@date: 2008-5-7
*	
*	Copyright (C) CTEC Xi'an JiaoTong University
*/

#include "stdafx.h"
#include "../AAFusion/AAFusion.h"
//#include <iostream>
#include <iomanip>

#if !defined _DEBUG
//#error ("this must be compiled in debug version")
#endif

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	std::cout<<setprecision(20);
	//RadarSampler *radarsampler = new RadarSampler(0, 0, 2000000.0);

	SceneInfo scene;
	scene.m_BottomLatitude = 30;
	scene.m_CentralLongitude = 21;
	scene.m_LeftLongitude = 18;
	scene.m_RightLongitude = 24;
	scene.m_ShipCount = 20;
	scene.m_TopLatitude = 60;

	F64 radarx, radary;
	GKProjection(scene.m_CentralLongitude, scene.m_CentralLongitude, 45, radarx, radary);
	RadarSampler *radarsampler = new RadarSampler(radarx, radary, 2000000.0f, NULL);
	AISSampler *aissampler = new AISSampler(radarx, radary);

	AADataGenerator *generator = new AADataGenerator(scene, radarsampler, aissampler);

	AAFusion *fusion = new AAFusion(scene.m_ShipCount, aissampler, radarsampler);
	
	S32 count = 200;
	while (count-- > 0)
	{
		::Sleep(1000);
		//aissampler->AcceptReader(fusion);	//read data from ais sampler
		//radarsampler->AcceptReader(fusion);
		//fusion->show();
	}

	delete fusion;
	delete radarsampler;
	delete aissampler;
	delete generator;
	std::cout<<"Ä£ÄâÍê±Ï£¡"<<std::endl;
	std::cin.ignore();
	return 0;
}