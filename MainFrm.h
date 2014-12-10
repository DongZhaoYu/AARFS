// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "WinSpliter.h"
#include "RadarPanel.h"
#include "AAFusionView.h"

#define WM_INIT (WM_USER + 100)

class CMainFrame : public CFrameWindowImpl<CMainFrame>, public CUpdateUI<CMainFrame>,
		public CMessageFilter, public CIdleHandler
{
protected:
	VerSplitter m_VSplitter;
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	CAARFSView m_view;
	AAFusionView m_fusionview;
	RadarPanel m_RadarPanel;

	CCommandBarCtrl m_CmdBar;

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
			return TRUE;

		return m_view.PreTranslateMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		UIUpdateToolBar();
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLbuttonDown)
		MESSAGE_HANDLER(WM_INIT, OnInit)
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
		COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// create command bar window
		//HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
		//// attach menu
		//m_CmdBar.AttachMenu(GetMenu());
		//// load command bar images
		//m_CmdBar.LoadImages(IDR_MAINFRAME);
		//// remove old menu
		//SetMenu(NULL);

		//HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

		//CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
		//AddSimpleReBarBand(hWndCmdBar);
		//AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

		//CreateSimpleStatusBar();

		const DWORD dwSplitStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		const DWORD dwExStyle = WS_EX_CLIENTEDGE;
		m_VSplitter.Create(*this, rcDefault, NULL, dwSplitStyle, dwExStyle);
		m_hWndClient = m_VSplitter;

		//m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);
		//m_view.Create(m_VSplitter, rcDefault, NULL);
		//m_rview.Create(m_VSplitter, rcDefault, NULL);
		//m_RadarMonitor.Create(m_VSplitter, _T("Radar Monitor"));
		m_RadarPanel.Create(m_VSplitter, _T("Fusion Panel"));
		m_fusionview.Create(m_RadarPanel);
		m_RadarPanel.SetClient(m_fusionview);

		m_view.Create(m_VSplitter, rcDefault, NULL);
		m_view.m_FusionWin = m_fusionview.m_hWnd;
		//m_view.Init();

		//m_RadarMonitor.SetPaneContainerExtendedStyle(PANECNT_NOCLOSEBUTTON);
		m_RadarPanel.SetPaneContainerExtendedStyle(PANECNT_NOCLOSEBUTTON);

		//m_view.Create(m_RadarPanel, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);

		m_VSplitter.SetSplitterPanes(m_view, m_RadarPanel);
		m_VSplitter.SetSplitterExtendedStyle(SPLIT_NONINTERACTIVE, SPLIT_NONINTERACTIVE);
		
		UpdateLayout();
		m_fusionview.SetNotifyWin(m_view.m_hWnd);
		m_VSplitter.m_cxyMin = 50;
		m_VSplitter.SetSplitterPos(640);

		//UIAddToolBar(hWndToolBar);
		//UISetCheck(ID_VIEW_TOOLBAR, 1);
		//UISetCheck(ID_VIEW_STATUS_BAR, 1);
		
		

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);



		//LONG style;
		//style = ::GetWindowLong(m_hWnd, GWL_STYLE);
		//style &= ~WS_MAXIMIZEBOX;
		//::SetWindowLong(m_hWnd, GWL_STYLE, style);
		//RECT rc;
		//GetWindowRect(&rc);
		//::SetWindowPos(m_hWnd, HWND_NOTOPMOST, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_DRAWFRAME);
		
		PostMessage(WM_INIT);
		::ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);
		return 0;
	}

	LRESULT OnInit(UINT, WPARAM , LPARAM, BOOL &bHandled)
	{
		m_view.Init();
		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);

		bHandled = FALSE;
		return 1;
	}

	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		PostMessage(WM_CLOSE);
		return 0;
	}

	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// TODO: add code to initialize document

		return 0;
	}

	LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		static BOOL bVisible = TRUE;	// initially visible
		bVisible = !bVisible;
		CReBarCtrl rebar = m_hWndToolBar;
		int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
		rebar.ShowBand(nBandIndex, bVisible);
		UISetCheck(ID_VIEW_TOOLBAR, bVisible);
		UpdateLayout();
		return 0;
	}

	LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
		::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
		UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
		UpdateLayout();
		return 0;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}

	LRESULT OnLbuttonDown(UINT msg, WPARAM wPara, LPARAM lPara, BOOL &bHandled)
	{
		return 0;
	}
};
