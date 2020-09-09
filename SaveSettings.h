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
#if !defined(AFX_SAVESETTINGS_H__8AFF31F8_AB6C_45B1_9BF6_4A202D99A55E__INCLUDED_)
#define AFX_SAVESETTINGS_H__8AFF31F8_AB6C_45B1_9BF6_4A202D99A55E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SaveSettings.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSaveSettings dialog

class CSaveSettings : public CDialog
{
// Construction
public:
	CSaveSettings(CWnd* pParent = NULL);   // standard constructor
	BOOL isOK;

// Dialog Data
	//{{AFX_DATA(CSaveSettings)
	enum { IDD = IDD_DIALOG2 };
	CEdit	m_newusersave;
	CListBox	m_userboxsave;
	CString	m_newusersave_str;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSaveSettings)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSaveSettings)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAVESETTINGS_H__8AFF31F8_AB6C_45B1_9BF6_4A202D99A55E__INCLUDED_)
