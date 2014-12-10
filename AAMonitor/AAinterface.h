/*
* @file: AAinterface.h
* @brief: an interface for monitor
* @author: dongzhaoyu
* @date: 2008-4-10
*
* Copyright (c) 2008 CTEC Xi'an JiaoTong University
*/

#if _MSC_VER > 1200
#pragma once
#define _AAINTERFACE_H
#endif

/*
*	This file is a definition of the interface for a radar monitor,
*	in the monitor we shall capture some user input and handle them.
*	And the windows message mechanism is used for implementation.
*
*/

#ifdef WIN32
#include <windows.h>
#pragma message("windows platform")
#else
#error "this must be complied in windows platform"
#endif

#include "..\AACommon\AAPredefine.h"
#include "..\AACommon\AAStdtype.h"
#include "..\AACommon\AANotifyMessage.h"
#include <Windows.h>

class AAMonitor
{
public:
	virtual void ShowMonitor() = 0;
	virtual void OnUpdateData(AANotifyMsg*) = 0;
	virtual Bool AddRect(const RECT &rc, const U32 color, const char *dscp) = 0;
	virtual Bool AddCircle(const POINT &center, const U32 r, const U32 color, const char *dscp) = 0;

protected:

};