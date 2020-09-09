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
// PointyLevel.cpp : implementation file
//

#include "stdafx.h"
#include "tomatoanalyzer.h"
#include "PointyLevel.h"
#include ".\pointylevel.h"
//#include ".\pointilevel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPointyLevel dialog


CPointyLevel::CPointyLevel(CWnd* pParent /*=NULL*/)
	: CDialog(CPointyLevel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPointyLevel)
	m_nMacroLevel = 0;
	m_nMicroLevel = 0;
	//}}AFX_DATA_INIT
}


void CPointyLevel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPointyLevel)
	DDX_Control(pDX, IDC_SLIDER_MICRO, m_Micro);
	DDX_Control(pDX, IDC_SLIDER_MACRO, m_Macro);
	DDX_Slider(pDX, IDC_SLIDER_MACRO, m_nMacroLevel);
	DDX_Slider(pDX, IDC_SLIDER_MICRO, m_nMicroLevel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPointyLevel, CDialog)
	//{{AFX_MSG_MAP(CPointyLevel)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_MACRO, OnNMCustomdrawSliderMacro)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_MICRO, OnNMCustomdrawSliderMicro)
	ON_STN_CLICKED(IDC_MICRO_VALUE, OnStnClickedMicroValue)
	ON_STN_CLICKED(IDC_MACRO_VALUE, OnStnClickedMacroValue)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPointyLevel message handlers

BOOL CPointyLevel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_Macro.SetRange(5, 40, TRUE);
    m_Macro.SetPos(m_nMacroLevel);
    m_Micro.SetRange(2, 10, TRUE);
    m_Micro.SetPos(m_nMicroLevel);
    SetDlgItemInt(IDC_MACRO_VALUE, m_nMacroLevel, FALSE);
    SetDlgItemInt(IDC_MICRO_VALUE, m_nMicroLevel, FALSE);
	
    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CPointyLevel::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	CSliderCtrl *   pSlider = (CSliderCtrl *)pScrollBar;
    
    int nMin = m_Micro.GetPos();
    int nMax = m_Macro.GetPos();

    if(pSlider == &m_Macro)
        SetDlgItemInt(IDC_MACRO_VALUE, nMax, FALSE);
    else
        SetDlgItemInt(IDC_MICRO_VALUE, nMin, FALSE);
    
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPointyLevel::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CPointyLevel::OnNMCustomdrawSliderMacro(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CPointyLevel::OnNMCustomdrawSliderMicro(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CPointyLevel::OnStnClickedMicroValue()
{
	// TODO: Add your control notification handler code here
}

void CPointyLevel::OnStnClickedMacroValue()
{
	// TODO: Add your control notification handler code here
}
