/* This file is part of Tomato Analyzer.

    Tomato Analyzer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Tomato Analyzer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Tomato Analyzer.  If not, see <http://www.gnu.org/licenses/>. */
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "AdvImage.h"
#include "BoundingBoxList.h"
#include "Slice.h"
#include "TomatoAnalyzer.h"

#include "MainFrm.h"
#include "TomatoAnalyzerDoc.h"
#include "SingleSliceView.h"
#include "TomatoAnalyzerView.h"
#include "SliceInfoFormView.h"
#include "MeasureItemList.h"
#include "FruitDataView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(TBN_DROPDOWN, AFX_IDW_TOOLBAR, OnSelectZoomLevel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

static UINT	nToolBarBtn[]={ID_FILE_OPEN,  
						   ID_EXPORT_DATA,
						   0,
						   ID_FILE_PREV,
						   ID_FILE_NEXT,
						   0,
						   ID_ZOOM,
						   0,
						   ID_ANALYZE_IMG,
						   ID_ANALYZE_COLOR, 
						   ID_MANUAL_REVISE
						};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	
	m_bSplitterWndInitialized=FALSE;
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}


void CMainFrame::OnSelectZoomLevel(NMHDR * pNotifyStruct, LRESULT * result)
{
	POINT	pt;
	GetCursorPos(&pt);

	CMenu	MunualMenu;
	MunualMenu.LoadMenu(IDR_ZOOMMENU);

	CMenu * pPopup = NULL;

	NMTOOLBAR *	pNt = (NMTOOLBAR *)pNotifyStruct;
	switch(pNt->iItem)
	{
	case ID_ZOOM:
		pPopup = MunualMenu.GetSubMenu(0);
		break;

	case ID_MANUAL_REVISE:
		pPopup = MunualMenu.GetSubMenu(1);
		break;
        
    case ID_SAVE_FRUIT:
        {
            char    szFileName[_MAX_PATH];
            char    szMenuItemString[_MAX_PATH];

            CTomatoAnalyzerDoc *    pDoc = (CTomatoAnalyzerDoc *)GetActiveDocument();
            pDoc->GetArchiveName(szFileName);
            lstrcpy(szMenuItemString, "Save to ");
            lstrcat(szMenuItemString, szFileName);

            pPopup = MunualMenu.GetSubMenu(3);
            pPopup->ModifyMenu(ID_FILE_SAVE, MF_BYCOMMAND|MF_STRING, ID_FILE_SAVE, szMenuItemString);
        }
        break;
	}

	if(NULL != pPopup)
		pPopup->TrackPopupMenu( TPM_LEFTALIGN|TPM_LEFTBUTTON,
								pt.x,
								pt.y,
								this
							  );

	MunualMenu.DestroyMenu();
	return;
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT|TBSTYLE_TRANSPARENT|TBSTYLE_LIST ) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	CToolBarCtrl& tbCtrl = m_wndToolBar.GetToolBarCtrl();
	tbCtrl.SetExtendedStyle( TBSTYLE_EX_DRAWDDARROWS );

	m_wndToolBar.SetButtonText (0,"Open Image");
	m_wndToolBar.SetButtonStyle(0, TBSTYLE_BUTTON|TBSTYLE_AUTOSIZE);

	m_wndToolBar.SetButtonText (1,"Save Fruit");
	m_wndToolBar.SetButtonStyle(1, TBSTYLE_DROPDOWN|TBSTYLE_AUTOSIZE);

	m_wndToolBar.SetButtonText (3,"Export Data");
	m_wndToolBar.SetButtonStyle(3, TBSTYLE_BUTTON|TBSTYLE_AUTOSIZE);

	m_wndToolBar.SetButtonText (5,"Fit size");
	m_wndToolBar.SetButtonStyle(5, TBSTYLE_DROPDOWN|TBSTYLE_AUTOSIZE);

	m_wndToolBar.SetButtonText (7,"Analyze");
	m_wndToolBar.SetButtonStyle(7, TBSTYLE_BUTTON|TBSTYLE_AUTOSIZE);

	m_wndToolBar.SetButtonText (8,"Color test");
	m_wndToolBar.SetButtonStyle(8, TBSTYLE_BUTTON|TBSTYLE_AUTOSIZE);

	m_wndToolBar.SetButtonText (9,"Revise");
	m_wndToolBar.SetButtonStyle(9, TBSTYLE_DROPDOWN|TBSTYLE_AUTOSIZE);


	if (!m_wndReBar.Create(this) ||
		!m_wndReBar.AddBar(&m_wndToolBar))
	{
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	// TODO: Remove this if you don't want tool tips
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);


	HICON theIcon;
	theIcon = AfxGetApp()->LoadIcon(IDR_OSU_TOMATO);
	SetIcon(theIcon, FALSE);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;


	return TRUE;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	if(SIZE_MINIMIZED != nType && m_bSplitterWndInitialized)
	{
		m_wndSplitWnd.SetColumnInfo(0, cx/2, 100);
		m_wndSplitRightHalf.SetRowInfo(0, cy*11/18, 100); 
	}

	// TODO: Add your message handler code here
	CFrameWnd::OnSize(nType, cx, cy);
	
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class
	//first split the window vertically into 2 half
	m_wndSplitWnd.CreateStatic(this, 1, 2);
	
	//create source code view
	m_wndSplitWnd.CreateView(
		0, 0, 
		RUNTIME_CLASS(CTomatoAnalyzerView),
		CSize(0, 0), 
		pContext
		);

	//split the right half vertically into 2 windows
	m_wndSplitRightHalf.CreateStatic(
		&m_wndSplitWnd, 
		2, 1, 
		WS_CHILD|WS_VISIBLE|WS_BORDER,
		m_wndSplitWnd.IdFromRowCol(0,1)
		);

	//create error view
	m_wndSplitRightHalf.CreateView(
		0, 0,
		RUNTIME_CLASS(CSingleSliceView),
		CSize(400, 400),
		pContext
		);

	//create object content view
	m_wndSplitRightHalf.CreateView(
		1, 0,
        //RUNTIME_CLASS(CSliceInfoFormView),
		RUNTIME_CLASS(CFruitDataView),     //
		CSize(400, 100),
		pContext
		);


	m_bSplitterWndInitialized=TRUE;

	return TRUE;
}
