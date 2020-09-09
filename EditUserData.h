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


// EditUserData dialog header file

class EditUserData : public CDialog
{
	DECLARE_DYNAMIC(EditUserData)

public:
	EditUserData(CWnd* pParent = NULL);   // standard constructor
	EditUserData(CWnd* pParent, std::vector<CString> columns, int numCols); // Overloaded Constructor
	virtual ~EditUserData();

// Dialog Data
	enum { IDD = IDD_EDITCOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog(); // Initializes the dialog objects

	DECLARE_MESSAGE_MAP()
public:
	int loopControl; // Controls how many editable boxes to make
	std::vector<CString> textEditCopy; // Used for the text in the boxes
	CString textEdit[30]; // Used to initialize the text in the boxes
	CEdit editBoxes[30]; // Objects corresponding to the text boxes
	afx_msg void OnBnClickedOk(); // Handles OK button
	afx_msg void OnBnClickedCancel(); // Handles Cancel button
};
