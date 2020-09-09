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
// PointiLevel.cpp : implementation file
//

#include "stdafx.h"
#include "tomatoanalyzer.h"
#include "PointiLevel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPointiLevel dialog


CPointiLevel::CPointiLevel(CWnd* pParent /*=NULL*/)
	: CDialog(CPointiLevel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPointiLevel)
	m_nMacroLevel = 0;
	m_nMicroLevel = 0;
	//}}AFX_DATA_INIT
}


void CPointiLevel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPointiLevel)
	DDX_Control(pDX, IDC_GRAPH, m_Graph);
	DDX_Control(pDX, IDC_SLIDER_MICRO, m_Micro);
	DDX_Control(pDX, IDC_SLIDER_MACRO, m_Macro);
	DDX_Slider(pDX, IDC_SLIDER_MACRO, m_nMacroLevel);
	DDX_Slider(pDX, IDC_SLIDER_MICRO, m_nMicroLevel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPointiLevel, CDialog)
	//{{AFX_MSG_MAP(CPointiLevel)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPointiLevel message handlers

BOOL CPointiLevel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_Macro.SetRange(1, 50, TRUE);
    m_Macro.SetPos(m_nMacroLevel);
    m_Micro.SetRange(1, 50, TRUE);
    m_Micro.SetPos(m_nMicroLevel);
    SetDlgItemInt(IDC_MACRO_VALUE, m_nMacroLevel, FALSE);
    SetDlgItemInt(IDC_MICRO_VALUE, m_nMicroLevel, FALSE);
	
    m_Graph.SetRange(m_nMicroLevel, m_nMacroLevel);
    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CPointiLevel::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	CSliderCtrl *   pSlider = (CSliderCtrl *)pScrollBar;
    
    int nMin = m_Micro.GetPos();
    int nMax = m_Macro.GetPos();

    if(pSlider == &m_Macro)
        SetDlgItemInt(IDC_MACRO_VALUE, nMax, FALSE);
    else
        SetDlgItemInt(IDC_MICRO_VALUE, nMin, FALSE);

    if(nMin < nMax)
        m_Graph.SetRange(nMin, nMax);
    
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
