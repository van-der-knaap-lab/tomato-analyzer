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
// UserAddData.cpp : implementation file
// "Add/Edit Data..." Dialog

#include "stdafx.h"
#include "TomatoAnalyzer.h"
#include "LABDialog.h"
#include "Resource.h"			// You have to include all of these to be able to include TomatoAnalyzerDoc.h...
#include "ProgressDialog.h"		//  
#include "BoundingBoxList.h"	//
#include "BinObjs.h"			//
#include "AdvImage.h"			//
#include "Slice.h"				//
#include "TomatoAnalyzer.h"		// ...
#include "TomatoAnalyzerDoc.h"
#include "UserAddData.h"
#include <sstream>

// UserAddData dialog

CPoint * UserAddData::m_topLeft = NULL; 

IMPLEMENT_DYNAMIC(UserAddData, CDialog)

// Default Constructor
UserAddData::UserAddData(CWnd* pParent /*=NULL*/)
	: CDialog(UserAddData::IDD, pParent)
	, box1("")
{
	box1 = "";
	m_chNum = 0;
	m_fnNum = 0;
	special = false;
	finished = false;
}

// Overloaded constructor
UserAddData::UserAddData(CWnd* pParent, std::vector<CString> tempVec, int numFruit,
						 CTomatoAnalyzerDoc* doc) : CDialog(UserAddData::IDD, pParent), box1("")
{
	box1 = "";
	m_chNum = 0;
	m_fnNum = 0;
	workingVec = tempVec;
	numberFruits = numFruit;
	special = true;
	finished = false;
	

	m_doc = doc; 
	m_doc->GetAllSlicesByNaturalSequence(m_slicePositions); 
}

// Destructor
UserAddData::~UserAddData()
{
}

// Performs the data exchange between the MFC objects and the variables (associates variables with objects)
void UserAddData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_comboColumnHeading);
	DDX_Control(pDX, IDC_COMBO2, m_comboFruitNumber);
	DDX_Control(pDX, IDC_EDIT1, m_editBox);
	DDX_Text(pDX, IDC_EDIT1, box1);
	DDX_CBIndex(pDX, IDC_COMBO1, m_chNum);
	DDX_CBIndex(pDX, IDC_COMBO2, m_fnNum);
}

// Links messages to handling functions
BEGIN_MESSAGE_MAP(UserAddData, CDialog)
	ON_BN_CLICKED(IDOK, &UserAddData::OnBnClickedOk)
	ON_BN_CLICKED(IDC_FINISH, &UserAddData::OnBnClickedFinish)
	ON_CBN_SELCHANGE(IDC_COMBO1, &UserAddData::OnCbnSelchangeCombo)
	ON_CBN_SELCHANGE(IDC_COMBO2, &UserAddData::OnCbnSelchangeCombo)
END_MESSAGE_MAP()

// Initializes the dialog window (mainly the combo boxes)
BOOL UserAddData::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_editBox.SetFocus(); // (supposedly) sets the focus of the dialog to the text box

	// TODO: Add extra initialization here
	// If the overloaded constructor was used, then use the following:
	if(special == true)
	{
		std::vector<CString>::iterator tempItr = workingVec.begin(); // workingVec has the column names
		// Loops through all entries in workingVec
		for(tempItr; tempItr < workingVec.end(); tempItr++)
		{
			// Converts the text from char* to CString and then adds the string to the combo box
			//CString temp;
			//temp.Format(_T("%s"), *tempItr);
			m_comboColumnHeading.AddString(*tempItr);
		}
		// Sets the displayed item for the combo box
		m_comboColumnHeading.SetCurSel(m_chNum);

		// USER MUST BE AWARE OF WHICH "FRUITS" ARE THE RULER(S) AND LABEL(S)
		// numberFruits contains the total number of fruits recognized by TA
		for(int i = 1; i <= numberFruits; i++)
		{
			// Converts text from char* to CString
			CString temp;
			if(i < 10)
				temp.Format(_T("Object 0%d"), i);
			else
				temp.Format(_T("Object %d"), i);
			// Adds entry to the combo box
			m_comboFruitNumber.AddString(temp);
		}
		// Sets displayed item
		m_comboFruitNumber.SetCurSel(m_fnNum);
		OnCbnSelchangeCombo();
	}
	// If there is no data, list defaults (precautionary case, should not be hit as window will only open when there are entries in the list)
	else
	{
		m_comboColumnHeading.AddString("Select a column heading...");
		m_comboColumnHeading.SetCurSel(m_chNum);
		m_comboFruitNumber.AddString("Select an object...");
		m_comboFruitNumber.SetCurSel(m_fnNum);
	}

	if (m_topLeft != NULL) 
		SetWindowPos(NULL, m_topLeft->x, m_topLeft->y, 0, 0, SWP_NOSIZE | SWP_NOZORDER); 

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void UserAddData::rememberPosition() {
	if (m_topLeft == NULL) 
		m_topLeft = new CPoint(); 
	CRect r; 
	GetWindowRect(r);  
	m_topLeft->x = r.TopLeft().x; 
	m_topLeft->y = r.TopLeft().y; 
}

// UserAddData message handlers

void UserAddData::OnBnClickedOk()
{
	// Default handler for the "OK" Button ("Next >" is the main OK button for this Dialog)
	rememberPosition(); 
	OnOK();
}

void UserAddData::OnBnClickedFinish()
{
	// Special handler for the "OK" button ("Finish" acts as OK but actually terminates the window)
	rememberPosition(); 
	finished = true;
	OnOK();
}

/// Called when the user picks an item from the dropdownlist
void UserAddData::OnCbnSelchangeCombo()
{
	// Grabs the row and column indices from the index values of the combo boxes
	int row = m_comboFruitNumber.GetCurSel();
	int col = m_comboColumnHeading.GetCurSel();

	// Sets the data to the box, if it's there
	CString meas(workingVec[col]); 
	CString data = m_doc->GetSlice(m_slicePositions[row])->getUserCreatedData(meas); 
	m_editBox.SetWindowTextA(data); 

	// Sets the active data entry slice to appear in the top right corner of TA's window
	CTomatoAnalyzerApp * pApp = (CTomatoAnalyzerApp * ) AfxGetApp();
    POSITION pos = pApp->GetFirstDocTemplatePosition();
    CDocTemplate * pDocTpl = pApp->GetNextDocTemplate(pos);
    if(NULL == pDocTpl)
        return;

    pos = pDocTpl->GetFirstDocPosition();
    CTomatoAnalyzerDoc *    pDoc = (CTomatoAnalyzerDoc *) pDocTpl->GetNextDoc(pos);
	pDoc->ShowObjectAddData(row);
}
