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
#if !defined(AFX_LOADSETTINGS1_H__74D4A1FA_7B54_411B_805C_8F5A244123DE__INCLUDED_)
#define AFX_LOADSETTINGS1_H__74D4A1FA_7B54_411B_805C_8F5A244123DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LoadSettings1.h : header file
//

#include "string.h"

/////////////////////////////////////////////////////////////////////////////
// CLoadSettings dialog

class CLoadSettings : public CDialog
{
// Construction
public:
	CLoadSettings(CWnd* pParent = NULL);   // standard constructor
	CLoadSettings(std::string userlist, CWnd* pParent =NULL);
	

// Dialog Data
	//{{AFX_DATA(CLoadSettings)
	enum { IDD = IDD_DIALOG1 };
	CListBox	m_userlistbox;
	CString	m_userslist;
	std::string user_str;
	int DoModal(std::string userlist);
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoadSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLoadSettings)
	afx_msg void OnSelchangeList1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOADSETTINGS1_H__74D4A1FA_7B54_411B_805C_8F5A244123DE__INCLUDED_)
