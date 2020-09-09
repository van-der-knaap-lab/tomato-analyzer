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
// UserDefinedDialog.cpp : implementation file
//

#include "stdafx.h"
#include "TomatoAnalyzer.h"
#include "UserDefinedDialog.h"
#include "EditUserData.h"

// UserDefinedDialog dialog

IMPLEMENT_DYNAMIC(UserDefinedDialog, CDialog)

// Constructor - Sets all of the text fields to be blank by default
UserDefinedDialog::UserDefinedDialog(CWnd* pParent /*=NULL*/)
	: CDialog(UserDefinedDialog::IDD, pParent)
	, box1("")
	, box2("")
	, box3("")
{
	box1 = "";
	box2 = "";
	box3 = "";
	text1 = "Added Columns:";
	finished = false;
	numUserCols = 0;
}

// Destructor
UserDefinedDialog::~UserDefinedDialog()
{
}

// Handles the data exchange processing between the framework and the code
void UserDefinedDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ADDF_MEASURE1, box1);
	DDX_Text(pDX, IDC_ADDF_MEASURE2, box2);
	DDX_Text(pDX, IDC_ADDF_MEASURE3, box3);
	DDX_Control(pDX, IDC_EDIT, editButton);
	DDX_Text(pDX, IDC_COLLIST, text1);
	DDX_Control(pDX, IDC_COLLIST, ColList);
}

// Event Message Map (MFC)
BEGIN_MESSAGE_MAP(UserDefinedDialog, CDialog)
ON_BN_CLICKED(IDOK, &UserDefinedDialog::OnBnClickedOk)
ON_BN_CLICKED(IDC_EDIT, &UserDefinedDialog::OnBnClickedEdit)
ON_BN_CLICKED(ID_OK1, &UserDefinedDialog::OnBnClickedOk1)
END_MESSAGE_MAP()


// UserDefinedDialog message handlers

BOOL UserDefinedDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (numUserCols > 0)
	{
		editButton.EnableWindow(TRUE);
	}

	MeasurementSet temp = ((CTomatoAnalyzerApp*)AfxGetApp())->getMeasurementSet();
	std::vector<CString> tempChar = temp.measurementsInGroup("User Created");
	text1 = "Added Columns:";
	text1 += "\r\n";
	for(int i = 0; i < tempChar.size(); i++)
	{
		if(i % 6 == 0 && i == 0)
			text1 += tempChar[i];
		else if (i % 6 == 0)
			text1 += "\r\n" + tempChar[i];
		else
			text1 += ", " + tempChar[i];
	}

	ColList.SetWindowText(text1);
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void UserDefinedDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
void UserDefinedDialog::OnBnClickedEdit()
{
	// TODO: Add your control notification handler code here
	MeasurementSet temp = ((CTomatoAnalyzerApp*)AfxGetApp())->getMeasurementSet();
	std::vector<CString> tempChar = temp.measurementsInGroup("User Created");
	int size = tempChar.size();
	bool blank = false;
	
	// Opens the Edit Column Dialog
	EditUserData dlg(NULL, tempChar, size);
	dlg.DoModal();

	// Handling code for Edit Button goes here
	for(int i = 0; i < size; i++)
	{
		CString temp = dlg.textEdit[i];
		if(temp.CompareNoCase(tempChar[i]) != 0)
		{
			if(temp.CompareNoCase("") == 0)
			{
				blank = true;
			}
			((CTomatoAnalyzerApp*)AfxGetApp())->editMeasurements(blank, temp, i);
		}
	}

	OnInitDialog();
}

void UserDefinedDialog::OnBnClickedOk1()
{
	// TODO: Add your control notification handler code here
	finished = true;
	OnOK();
}
