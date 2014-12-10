/*	@file: AAGUI.h
*	@brief: the definition of gui controls used in AARFS
*	@author: dongzhaoyu
*	@date: 2008-4-23
*	
*	Copyright (C) CTEC Xi'an JiaoTong University
*/

#if _MSC_VER > 1200
#pragma once
#define _AAGUI_H
#endif

#include <Windows.h>
//#include "AAinterface.h"
#include "AAElements.h"
#include <list>
#include "..\AACommon\AAPredefine.h"
#include "AADX.h"

#define WM_GUIEVENT	(WM_USER + 101)

typedef enum _AAGUIType
{
	AAGUIT_STATIC,
	AAGUIT_BUTTON,
	AAGUIT_LISTBOX
}AAGUIType;

class AAEventHandler
{
public:
	virtual Bool OnInit() { return TRUE;}

	//the windows message handler definition
	virtual Bool MsgPro(U32 Msg, WPARAM wpara, LPARAM lpara) { return FALSE;}

	//the mouse keyboard message handler
	virtual Bool HandleKeyboard(U32 Msg, WPARAM wpara, LPARAM lpara) { return FALSE;}
	virtual Bool HandleMouse(U32 Msg, /*POINT point, */WPARAM wparam, LPARAM lparam) { return FALSE;}

	virtual void OnFocusIn() { m_bHasFocus = TRUE;}
	virtual void OnFocusOut() { m_bHasFocus = FALSE; }
	virtual void OnMouseEnter() { m_bMouseOver = TRUE;}
	virtual void OnMouseLeave() { m_bMouseOver = FALSE;}

	SET_GET_ACCESSOR(Bool, bEnable)

protected:
	AAEventHandler(Bool = FALSE, Bool = FALSE, Bool = TRUE);

	Bool m_bMouseOver;	//the mouse pointer is on the handler
	Bool m_bHasFocus;	//the handler has the focus
	Bool m_bEnable;		//the handler is able to work or not
};

class AAGUIControl : public AAEventHandler
{
public:
	virtual ~AAGUIControl();
	
	virtual void Refresh() {}
	virtual void Render() {}
	virtual Bool ContainPoint(const POINT &p) const { return PtInRect(&m_RectBox, p);}

	void SetPosition(U32 nx, U32 ny) { m_X = nx; m_Y = ny; Update();}
	void SetHW(U32 height, U32 width) { m_Height = height; m_Width = width; Update();}
	virtual void SetUserData(void *data) { m_pUserData = data;}

	GET_ACCESSOR(const void*, pUserData)
	SET_GET_ACCESSOR(U32, ID)
	GET_ACCESSOR(AAGUIType, Type)

protected:
	AAGUIControl(PAADX aadx = NULL, U32 id = 0);
	virtual void Update() {}
	
	U32 m_X, m_Y, m_Height, m_Width;		//the left top corner coordinate and the height and width
	AAGUIType m_Type;
	void *m_pUserData;						//the data is associated by the control, it is set by the user
	U32 m_ID;								//here we use id as an index to access the control
	CGrowableArray<Element*> m_lElements;	//the elements list contained in the UI control
	RECT m_RectBox;
	PAADX m_pAADX;
};

class AAGUIStatic : public AAGUIControl
{
public:
	AAGUIStatic();
	AAGUIStatic(AAText::INFO &tinfo, PAADX aadx, U32 id);
	
	virtual ~AAGUIStatic() {};
	virtual void SetUserData(void *data);
	virtual void Render();

protected:
	virtual void Update();

	D3DXCOLOR m_Color;
};

class AAGUIButton : public AAGUIStatic
{
public:
	AAGUIButton();
	AAGUIButton(AAText::INFO &tinfo, PAADX aadx, U32 id);

	virtual ~AAGUIButton();
	virtual void Render();
	virtual Bool HandleMouse(U32 Msg, /*POINT point, */WPARAM wparam, LPARAM lparam);

protected:
	Bool m_bPressed;
	//D3DXCOLOR m_ColorA;
	//D3DXCOLOR m_ColorB;
};