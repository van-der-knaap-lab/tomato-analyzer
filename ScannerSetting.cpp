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
// ScannerSetting.cpp : implementation file
//

#include "stdafx.h"
#include "tomatoanalyzer.h"
#include "ScannerSetting.h"
#include ".\scannersetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScannerSetting dialog


CScannerSetting::CScannerSetting(CWnd* pParent /*=NULL*/)
	: CDialog(CScannerSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScannerSetting)
	m_dpi = 0;
	m_iUnit = 0;
	//}}AFX_DATA_INIT
	
}


void CScannerSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScannerSetting)
	DDX_Control(pDX, IDC_UNIT_SELECTION, m_comboUnitName);
	DDX_Text(pDX, IDC_SCANNER_DPI, m_dpi);
	DDX_CBIndex(pDX, IDC_UNIT_SELECTION, m_iUnit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CScannerSetting, CDialog)
	//{{AFX_MSG_MAP(CScannerSetting)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_SCANNER_DPI, OnEnChangeScannerDpi)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScannerSetting message handlers

void CScannerSetting::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

BOOL CScannerSetting::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_comboUnitName.AddString("Pixel");
	m_comboUnitName.AddString("Millimeter");
	m_comboUnitName.AddString("Centimeter");
	m_comboUnitName.AddString("Inch");
    m_comboUnitName.SetCurSel(m_iUnit);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CScannerSetting::OnEnChangeScannerDpi()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
