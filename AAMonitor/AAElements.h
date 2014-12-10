/*
*	@file: AAElements.h
*	@brief: the elements used by a monitor
*	@author: dongzhaoyu
*	@date: 2008-4-16
*
*	Copyright (C) 2008 CTEC Xi'an JiaoTong University
*/

#if _MSC_VER > 1200
#pragma once
#define _AAELEMENTS_H
#endif

#include <windows.h>
#include "..\AACommon\AAStdtype.h"
#include "AADX.h"
#include <list>
#include <string>

class Element
{
public:
	typedef std::list<Element*> Element_List_t;
	typedef std::list<Element*>::iterator Element_List_it_t;

	virtual void Draw(U32 = 0xaaffffff) const = 0;
	virtual Bool IsIn(const POINT&) const = 0;
	virtual void GetPos(RECT &rc) const = 0;
	virtual void SetPos(U32 x, U32 y) {}
	virtual void SetColor(U32 color) {}
	virtual void SetData(void *str) {}
	virtual void SetAngle(F32 angle) {}
};

class AALine : public Element
{
public:
	AALine();
	AALine(const POINT &start, const POINT &end, U32 color, const char *desc, PAADX aadx);

	virtual ~AALine();
	virtual void Draw(U32 = 0xaaffffff) const;
	virtual Bool IsIn(const POINT &p) const { return TRUE;};
	virtual void GetPos(RECT &rc) const {};

private:
	std::string m_sTitle;
	AALineImpl::PAALineImpl m_pImpl;
};

class AARect : public Element
{
public:
	AARect();
	AARect(const RECT&, U32 color, const char *dscp, const PAADX aadx);

	virtual ~AARect();
	virtual void Draw(U32 = 0xaaffffff) const ;
	virtual Bool IsIn(const POINT&) const;
	virtual void GetPos(RECT &rc) const;

	const std::string& GetTitle() const;

private:
	std::string m_sTitle;
	PAARectImpl m_pImpl;
};

class AACircle : public Element
{
public:
	AACircle();
	AACircle(const POINT &p, U32 r, U32 color, const char *dscp, const PAADX aadx);

	virtual ~AACircle();
	virtual void Draw(U32 = 0xaaffffff) const;
	virtual Bool IsIn(const POINT&) const;
	virtual void GetPos(RECT &rc) const;
	const std::string& GetTitle() const;

	U32 GetRadius() const  { return m_pImpl->GetRadius();}
	const POINT GetCenter() const { return m_pImpl->GetCenter();}
	U32 GetColor() const { return m_pImpl->GetColor();}

private:
	std::string m_sTitle;
	AACircleImpl *m_pImpl;
};

class AAText : public Element
{
public:
	typedef AATextImpl::TEXTINFO INFO;

	AAText();
	AAText(const INFO &info, const char *title, const PAADX aadx);

	virtual ~AAText();
	virtual void Draw(U32 = 0xaaffffff) const;
	virtual Bool IsIn(const POINT&) const;
	virtual void GetPos(RECT &rc) const;
	const std::string& GetTitle() const;
	virtual void SetPos(U32 x, U32 y);
	virtual void SetColor(U32 color);
	virtual void SetData(void *str);

private:
	std::string m_sTitle;
	AATextImpl::PAATextImpl m_pImpl;
};

class AADial : public Element
{
public:
	typedef AADial* PAADial;

	AADial();
	AADial(Element* element, U32 color, PAADX aadx, F32 angle);

	virtual ~AADial();
	virtual void Draw(U32 = 0xaaffffff) const;
	virtual void GetPos(RECT &rc) const {}
	virtual Bool IsIn(const POINT &p) const { return TRUE;}
	virtual void SetAngle(F32 angle) { m_Angle = angle;}

private:
	Element *m_pElement;
	AADialImpl *m_pImpl;
	F32 m_Angle;
};