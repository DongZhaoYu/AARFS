/*
* @file: AAType.h
* @brief: Window splitter used in AARFS
* @author: dongzhaoyu
* @date: 2008-4-10
*
* Copyright (c) 2008 CTEC Xi'an JiaoTong University
*/

#ifndef _RADARPANEL_H
#define _RADARpANEL_H

class RadarPanel : public CPaneContainerImpl<RadarPanel>
{
public:
    DECLARE_WND_CLASS_EX(_T("My_PaneContainer"), 0, -1)

	//BEGIN_MSG_MAP(RadarPanel)
	//	MESSAGE_HANDLER(WM_PAINT, OnPaint)
	//END_MSG_MAP()

    RadarPanel() : m_bColoredHeader(false)
    { }

    // Operations
    void EnableColoredHeader ( bool bEnable = true );

    // Overrides
    BOOL GetToolTipText(LPNMHDR lpnmh);
    void DrawPaneTitle(CDCHandle dc);
    HFONT GetTitleFont();

    // NOTE: There is a bug in CPaneContainer that prevents this from being
    // called after you switch the container to vertical mode. Change
    // CalcSize() to pT->CalcSize() in CPaneContainer::SetPaneContainerExtendedStyle()
    void CalcSize() { m_cxyHeader = 30; }

	//LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	//{
	//	SendMessage(uMsg, wParam, lParam);
	//	return 0;
	//}

protected:
    bool m_bColoredHeader;
    CFont m_titleFont;
};


#endif