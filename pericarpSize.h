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
#if !defined(AFX_PERICARPSIZE_H__C1952F1C_CD31_4513_BB9F_DAC7C75FAA00__INCLUDED_)
#define AFX_PERICARPSIZE_H__C1952F1C_CD31_4513_BB9F_DAC7C75FAA00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// pericarpSize.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// pericarpSize dialog

class pericarpSize : public CDialog
{
// Construction
public:
	pericarpSize(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(pericarpSize)
	enum { IDD = IDD_PERICARPSIZE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(pericarpSize)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(pericarpSize)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PERICARPSIZE_H__C1952F1C_CD31_4513_BB9F_DAC7C75FAA00__INCLUDED_)
