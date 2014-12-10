/*
* @file: AAType.h
* @brief: Window splitter used in AARFS
* @author: dongzhaoyu
* @date: 2008-4-10
*
* Copyright (c) 2008 Xi'an JiaoTong University
*/

#ifndef _WINSPLITER_H
#define _WINSPLITER_H


template <bool t_bVertical = true>
class WinSpliter : public CSplitterWindowImpl<WinSpliter<t_bVertical>, t_bVertical>
{
public:
    DECLARE_WND_CLASS_EX(_T("My_SplitterWindow"), CS_DBLCLKS, COLOR_WINDOW)

    WinSpliter() : m_bPatternBar(false)
    { }
    
    // Operations
    void EnablePatternBar ( bool bEnable = false )
    {
        if ( bEnable != m_bPatternBar )
            {
            m_bPatternBar = bEnable;
            UpdateSplitterLayout();
            }
    }

    // Overrideables
    void DrawSplitterBar(CDCHandle dc)
    {
    RECT rect;

        // If we're not using a colored bar, let the base class do the
        // default drawing.
        if ( !m_bPatternBar )
            {
            CSplitterWindowImpl<WinSpliter<t_bVertical>, t_bVertical>::DrawSplitterBar(dc);
            return;
            }

        // Create a brush to paint with, if we haven't already done so.
        if ( m_br.IsNull() )
            m_br.CreateHatchBrush ( HS_DIAGCROSS, 
                                    t_bVertical ? RGB(255,0,0) : RGB(0,0,255) );

        if ( GetSplitterBarRect ( &rect ) )
        {
            dc.FillRect ( &rect, m_br );

            // draw 3D edge if needed
            if ( (GetExStyle() & WS_EX_CLIENTEDGE) != 0)
                dc.DrawEdge(&rect, EDGE_RAISED, t_bVertical ? (BF_LEFT | BF_RIGHT) : (BF_TOP | BF_BOTTOM));
        }
    }

protected:
    CBrush m_br;
    bool   m_bPatternBar;
};

typedef WinSpliter<true>    VerSplitter;
typedef WinSpliter<false>   HorSplitter;

#endif