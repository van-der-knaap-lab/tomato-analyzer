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

// UserDefinedDialog dialog

class UserDefinedDialog : public CDialog
{
	DECLARE_DYNAMIC(UserDefinedDialog)

public:
	UserDefinedDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~UserDefinedDialog();

// Dialog Data
	enum { IDD = IDD_ADDFIELD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog(); // Initializes the dialog objects

	DECLARE_MESSAGE_MAP()
public:
	CString box1, box2, box3, text1; // Variables that contain the data from the text boxes
	bool finished;
	int numUserCols;
	CEdit ColList;
	CButton editButton;
	afx_msg void OnBnClickedOk(); // Functions that handle when Ok or Cancel are clicked
	afx_msg void OnBnClickedEdit();
	afx_msg void OnBnClickedOk1();
};
