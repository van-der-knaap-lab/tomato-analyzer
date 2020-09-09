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
#pragma once
#include "afxwin.h"


// CLoadSettings dialog

class CLoadSettings : public CDialog
{
	DECLARE_DYNAMIC(CLoadSettings)

public:
	CLoadSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLoadSettings();

	enum { IDD = IDD_LOADSETTINGS };
	CListBox UserListBox;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
//{{AFX_MSG(CLoadSettings)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCancel();
};
