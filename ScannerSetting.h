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
#include "afxcmn.h"
#if !defined(AFX_SCANNERSETTING_H__6F91C80E_D013_4BC6_B931_CF96506E7B18__INCLUDED_)
#define AFX_SCANNERSETTING_H__6F91C80E_D013_4BC6_B931_CF96506E7B18__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScannerSetting.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScannerSetting dialog

class CScannerSetting : public CDialog
{
// Construction
public:
	CScannerSetting(CWnd* pParent = NULL);   // standard constructor

public:

// Dialog Data
	//{{AFX_DATA(CScannerSetting)
	enum { IDD = IDD_SCANNER_DPI };
	CComboBox	m_comboUnitName;
	int		m_dpi;
	int		m_iUnit;
	bool    manualDpi;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScannerSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScannerSetting)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CSliderCtrl brightCtrl;
	afx_msg void OnEnChangeScannerDpi();
	afx_msg void OnBnClickedButton1();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCANNERSETTING_H__6F91C80E_D013_4BC6_B931_CF96506E7B18__INCLUDED_)
