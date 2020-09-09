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
// PCAPOINTS.cpp : implementation file
//

#include "stdafx.h"
#include "TomatoAnalyzer.h"
#include "PCAPOINTS.h"

const int MIN_PCA_POINTS = 4; 
const int MAX_PCA_POINTS = 200;	

// PCAPOINTS dialog

IMPLEMENT_DYNAMIC(PCAPOINTS, CDialog)
PCAPOINTS::PCAPOINTS(CWnd* pParent /*=NULL*/)
	: CDialog(PCAPOINTS::IDD, pParent)
	, PCA_Value(10)
{
	clickSave = false;
}

PCAPOINTS::~PCAPOINTS()
{
}

BOOL PCAPOINTS::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	PCA_Slider.SetRange(MIN_PCA_POINTS / 2, MAX_PCA_POINTS / 2, TRUE);
	sliderValue = PCA_Value / 2; 
    PCA_Slider.SetPos(sliderValue);
    SetDlgItemInt(IDC_STATIC1, PCA_Value, FALSE);
	
    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void PCAPOINTS::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PCAPOINTS)
	DDX_Control(pDX, IDC_SLIDER1, PCA_Slider);
	DDX_Slider(pDX, IDC_SLIDER1, sliderValue);
	//}}AFX_DATA_MAP
}

void PCAPOINTS::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	CSliderCtrl *   pSlider = (CSliderCtrl *)pScrollBar;
    
    int n = PCA_Slider.GetPos();
    SetDlgItemInt(IDC_STATIC1, n * 2, FALSE);
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);

	UpdateData(true); 
	PCA_Value = sliderValue * 2; 
}


BEGIN_MESSAGE_MAP(PCAPOINTS, CDialog)

	//{{AFX_MSG_MAP(PCAPOINTS)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, OnNMCustomdrawSlider)
END_MESSAGE_MAP()

void PCAPOINTS::OnNMCustomdrawSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	*pResult = 0;
}

void PCAPOINTS::OnBnClickedOk()
{
	clickSave = true;
	OnOK();
}


// PCAPOINTS message handlers
