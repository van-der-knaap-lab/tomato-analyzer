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
// EllipseSize.cpp : implementation file
//

#include "stdafx.h"
#include "TomatoAnalyzer.h"
#include "EllipseSize.h"


// EllipseSize dialog

IMPLEMENT_DYNAMIC(EllipseSize, CDialog)
EllipseSize::EllipseSize(CWnd* pParent /*=NULL*/)
	: CDialog(EllipseSize::IDD, pParent)
	, Ellipse_Value(90)
{
	clickSave = false;
}

EllipseSize::~EllipseSize()
{
}

BOOL EllipseSize::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	Ellipse_Slider.SetRange(60, 100, TRUE);
    Ellipse_Slider.SetPos(Ellipse_Value);
    SetDlgItemInt(IDC_STATIC1, Ellipse_Value, FALSE);
	
    return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void EllipseSize::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PCAPOINTS)
	DDX_Control(pDX, IDC_SLIDER1, Ellipse_Slider);
	DDX_Slider(pDX, IDC_SLIDER1, Ellipse_Value);
	//}}AFX_DATA_MAP
}

void EllipseSize::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	CSliderCtrl *   pSlider = (CSliderCtrl *)pScrollBar;
    
    int n = Ellipse_Slider.GetPos();

        SetDlgItemInt(IDC_STATIC1, n, FALSE);
    
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


BEGIN_MESSAGE_MAP(EllipseSize, CDialog)

	//{{AFX_MSG_MAP(EllipseSize)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, OnNMCustomdrawSlider)
END_MESSAGE_MAP()

void EllipseSize::OnNMCustomdrawSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void EllipseSize::OnBnClickedOk()
{
	clickSave = true;
	// TODO: Add your control notification handler code here
	OnOK();
}


// EllipseSize message handlers
