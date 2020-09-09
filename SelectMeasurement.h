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
#if !defined(AFX_SELECTMEASUREMENT_H__72BE64BE_BCF1_4735_A49D_1745294FE2F5__INCLUDED_)
#define AFX_SELECTMEASUREMENT_H__72BE64BE_BCF1_4735_A49D_1745294FE2F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectMeasurement.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectMeasurement dialog

class CSelectMeasurement : public CDialog
{
// Construction
public:
	CSelectMeasurement(CWnd* pParent = NULL);   // standard constructor

//Attributes
public:
    std::vector<int>    m_vctSelectedItem;
	afx_msg void OnSelectAll();
	virtual BOOL OnInitDialog();

// Dialog Data
	//{{AFX_DATA(CSelectMeasurement)
	enum { IDD = IDD_SELECT_MEASUREMENT };
	CCheckboxTreeCtrl	m_ctrTree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectMeasurement)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectMeasurement)
	virtual void OnOK();
	//virtual BOOL OnInitDialog();
	//afx_msg void OnSelectAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTMEASUREMENT_H__72BE64BE_BCF1_4735_A49D_1745294FE2F5__INCLUDED_)
