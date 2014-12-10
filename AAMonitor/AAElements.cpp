/*
*	@file: AAElements.cpp
*	@brief: the implementation of basic elements in radar monitor including 
*			rectanglar, circle and text for now
*	@author: dongzhaoyu
*	@date: 2008-4-16
*
*	Copyright (C) 2008 CTEC Xi'an JiaoTong University
*/

#include "AAElements.h"
#include <assert.h>

//------------------------------------------------------------------------------------------
//implementation of AALine
//------------------------------------------------------------------------------------------
AALine::AALine() : m_pImpl(NULL)
{}

AALine::AALine(const POINT &start, const POINT &end, U32 color, const char *desc, PAADX aadx)
{
	m_sTitle = std::string(desc);
	m_pImpl = new AALineImpl(start, end, color, aadx);
}

AALine::~AALine()
{
	if (m_pImpl != NULL)
		delete m_pImpl;
	m_pImpl = NULL;
}

void AALine::Draw(U32 color) const 
{
	if (m_pImpl != NULL)
		m_pImpl->Draw();
}
//------------------------------------------------------------------------------------------
//implementation of AARect
//------------------------------------------------------------------------------------------
AARect::AARect() : m_pImpl(NULL)
{}

AARect::AARect(const RECT &rc, U32 color, const char *dscp, const PAADX aadx)
{
	m_sTitle = std::string(dscp);
	m_pImpl = new AARectImpl(aadx, color, rc);
}

AARect::~AARect()
{
	if (m_pImpl != NULL)
		delete m_pImpl;
	m_pImpl = NULL;
}

void AARect::Draw(U32 color) const
{
	if (m_pImpl != NULL)
		m_pImpl->Draw();
}

Bool AARect::IsIn(const POINT &p) const
{
	if (m_pImpl != NULL)
		return m_pImpl->IsIn(p);
	return FALSE;
}

void AARect::GetPos(RECT &rc) const
{
	if (m_pImpl != NULL)
		m_pImpl->GetPos(rc);
}

const std::string& AARect::GetTitle() const
{
	return this->m_sTitle;
}

//-------------------------------------------------------------------------------------------------------
//the implementation of AACircle
//-------------------------------------------------------------------------------------------------------
AACircle::AACircle() : m_pImpl(NULL)
{}

AACircle::AACircle(const POINT &p, U32 r, U32 color, const char *dscp, const PAADX aadx)
{
	m_sTitle = std::string(dscp);
	m_pImpl = new AACircleImpl(p, r, color, aadx);
}

AACircle::~AACircle()
{
	if (m_pImpl != NULL)
		delete m_pImpl;
	m_pImpl = NULL;
}

void AACircle::Draw(U32 color) const
{
	if (m_pImpl != NULL)
		m_pImpl->Draw();
}

Bool AACircle::IsIn(const POINT &p) const
{
	if (m_pImpl != NULL)
		return m_pImpl->IsIn(p);
	return FALSE;
}

void AACircle::GetPos(RECT &rc) const
{
	if (m_pImpl != NULL)
		m_pImpl->GetPos(rc);
}

const std::string& AACircle::GetTitle() const
{
	return m_sTitle;
}

//----------------------------------------------------------------------------------------------------------
//the implementation of AAText
//----------------------------------------------------------------------------------------------------------
AAText::AAText() : m_pImpl(NULL)
{}

AAText::AAText(const AAText::INFO &info, const char *title, const PAADX aadx)
{
	m_sTitle = std::string(title);
	m_pImpl = new AATextImpl(info, aadx);
}

AAText::~AAText()
{
	if (m_pImpl != NULL)
		delete m_pImpl;
	m_pImpl = NULL;
}

void AAText::Draw(U32 color) const
{
	if (m_pImpl != NULL)
		m_pImpl->Draw(color);
}

Bool AAText::IsIn(const POINT &p) const
{
	return m_pImpl->IsIn(p);
}

void AAText::GetPos(RECT &rc) const
{
	m_pImpl->GetRectBox(rc);
}

const std::string& AAText::GetTitle() const
{
	return m_sTitle;
}

void AAText::SetPos(U32 x, U32 y)
{
	m_pImpl->SetPosx(x);
	m_pImpl->SetPosy(y);
}

void AAText::SetData(void *str)
{
	m_pImpl->SetString((TCHAR*)str);
}

void AAText::SetColor(U32 color)
{
	m_pImpl->SetColor(color);
}

//--------------------------------------------------------------------------------------------------------------
//the implementation of AADial
//--------------------------------------------------------------------------------------------------------------
AADial::AADial() : m_pImpl(NULL), m_pElement(NULL)
{}

AADial::AADial(Element* element, U32 color, PAADX aadx, F32 angle) : m_pElement(element),
																	m_Angle(angle)
{
	assert(element != NULL);
	const POINT center = (static_cast<AACircle*>(element))->GetCenter();
	U32 radius = (static_cast<AACircle*>(element))->GetRadius();
	m_pImpl = new AADialImpl(center, radius, color, aadx);
}

AADial::~AADial()
{
	if (m_pElement != NULL)	//we release the element here
		delete m_pElement;
	m_pElement = NULL;
	if (m_pImpl != NULL)
		delete m_pImpl;
	m_pImpl = NULL;
}

void AADial::Draw(U32 color) const
{
	//static F32 angle = m_Angle;
	m_pElement->Draw();
	if (m_pImpl != NULL)
		m_pImpl->Draw(m_Angle);
	//angle +=1;
}