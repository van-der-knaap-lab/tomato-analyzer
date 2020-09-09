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
#if !defined(AFX_POINTYLEVEL_H__52B8D696_02EC_4569_BA6E_7E3554412A6D__INCLUDED_)
#define AFX_POINTYLEVEL_H__52B8D696_02EC_4569_BA6E_7E3554412A6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PointyLevel.h : header file
//
//#include "PointyLevelGraph.h"

/////////////////////////////////////////////////////////////////////////////
// CPointyLevel dialog

class CPointyLevel : public CDialog
{
// Construction
public:
	CPointyLevel(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPointyLevel)
	enum { IDD = IDD_LEVEL_CONTROL1 };
	CSliderCtrl	m_Micro;
	CSliderCtrl	m_Macro;
	int		m_nMacroLevel;
	int		m_nMicroLevel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPointyLevel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPointyLevel)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMCustomdrawSliderMacro(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderMicro(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnStnClickedMicroValue();
	afx_msg void OnStnClickedMacroValue();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POINTYLEVEL_H__52B8D696_02EC_4569_BA6E_7E3554412A6D__INCLUDED_)
