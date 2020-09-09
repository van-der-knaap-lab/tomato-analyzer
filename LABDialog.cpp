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
// LABDialog.cpp : implementation file
// Class for the Enter L*a*b* dialog

#include "stdafx.h"
#include <fstream> 
#include <sstream>
using namespace std; 

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


// LABDialog dialog

IMPLEMENT_DYNAMIC(LABDialog, CDialog)

// Default Constructor
LABDialog::LABDialog(CWnd* pParent /*=NULL*/)
	: CDialog(LABDialog::IDD, pParent)
{
	m_TileNum = 0;
}

// Destructor
LABDialog::~LABDialog()
{
}

// Performs the data exchange between controls and variables
void LABDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	// Maps control structures to the objects on the dialog
	DDX_Control(pDX, IDC_PALETTE, m_comboTiles);
	// Maps the data variables to the controls
	DDX_Text(pDX, IDC_LSTAR_BOX, lBox);
	DDX_Text(pDX, IDC_ASTAR_BOX, aBox);
	DDX_Text(pDX, IDC_BSTAR_BOX, bBox);
	// Associates an integer to the combo box for its index
	DDX_CBIndex(pDX, IDC_PALETTE, m_TileNum);
}


BEGIN_MESSAGE_MAP(LABDialog, CDialog)
	ON_CBN_SELCHANGE(IDC_PALETTE, &LABDialog::OnCbnSelchangePalette)
	ON_BN_CLICKED(IDC_NEXT, &LABDialog::OnBnClickedNext)
	ON_BN_CLICKED(IDC_FINISH, &LABDialog::OnBnClickedFinish)
	ON_BN_CLICKED(IDC_FILE, &LABDialog::OnBnClickedFile)
END_MESSAGE_MAP()

// Initializes the dialog window (mainly the combo boxes)
BOOL LABDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CTomatoAnalyzerApp * pApp = (CTomatoAnalyzerApp * ) AfxGetApp();
	doc = pApp->GetCurrentDoc(); 
	doc->GetSelectedSliceByNaturalSequence(slicePos);
	
	for (int i = 1; i <= slicePos.size(); i++) {
		CString tmp; 
		tmp.Format(_T("Tile %02d"), i); 
		m_comboTiles.AddString(tmp); 
	}

	readFromCurrentTile(); 

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void LABDialog::readFromCurrentTile() {
	CSlice * slice = doc->GetSlice(slicePos[m_TileNum]); 
	lBox = slice->getExpectedL(); 
	aBox = slice->getExpectedA(); 
	bBox = slice->getExpectedB(); 
	UpdateData(false); 
	
	m_comboTiles.SetCurSel(m_TileNum);
	doc->ShowTiles(m_TileNum);
	CEdit * lEdit = reinterpret_cast<CEdit *>(GetDlgItem(IDC_LSTAR_BOX));
	lEdit->SetFocus(); 
	lEdit->SetSel(0, -1); 
}

void LABDialog::writeToCurrentTile() {
	UpdateData(true); 
	CSlice * slice = doc->GetSlice(slicePos[m_TileNum]); 
	slice->setExpectedL(lBox); 
	slice->setExpectedA(aBox); 
	slice->setExpectedB(bBox); 
}


// LABDialog message handlers

/// Updates the dialog when the user selects a different tile. 
void LABDialog::OnCbnSelchangePalette()
{
	m_TileNum = m_comboTiles.GetCurSel();
	readFromCurrentTile(); 
}

// When the "Next" button is pressed
void LABDialog::OnBnClickedNext()
{
	writeToCurrentTile(); 
	m_TileNum = (m_TileNum + 1) % slicePos.size(); 
	readFromCurrentTile(); 
}

// When the "Finish" button is pressed
void LABDialog::OnBnClickedFinish()
{
	writeToCurrentTile(); 
	OnOK();
}

void LABDialog::OnBnClickedFile()
{
	CFileDialog csvDlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, _T("CSV Files (*.csv)|*.csv|All (*.*)|*.*||")); 
	int BUFFER_LEN = 1024;
	CString fileName; 
	csvDlg.GetOFN().lpstrFile = fileName.GetBuffer(BUFFER_LEN);
	csvDlg.GetOFN().nMaxFile = BUFFER_LEN;
	INT_PTR ret = csvDlg.DoModal(); 
	fileName.ReleaseBuffer(); 

	if (ret == IDOK) {
		fstream in(fileName);
		vector<string> headers; 
		bool readSucc = splitNextLine(in, headers); 
		if (! readSucc) {
			AfxMessageBox("Could not read the first line of the CSV file.", MB_ICONEXCLAMATION); 
			return; 
		}
		int lCol = -1, aCol = -1, bCol = -1; 
		for (int i = 0; i < headers.size(); i++) {
			if (headers[i] == "Expected L") {
				lCol = i; 
			} else if (headers[i] == "Expected A") {
				aCol = i; 
			} else if (headers[i] == "Expected B") { 
				bCol = i; 
			}
		}
		if (lCol == -1 || aCol == -1 || bCol == -1) {
			AfxMessageBox("Could not find headings in the CSV file for \"Expected L\", \"Expected A\", or \"Expected B\".", 
				MB_ICONEXCLAMATION); 
			return; 
		}
		vector<double> l, a, b; 
		vector<string> cols; 
		for (int i = 0; ; i++) {
			readSucc = splitNextLine(in, cols); 
			if (! readSucc || cols.size() < lCol + 1 || cols.size() < aCol + 1 || cols.size() < bCol + 1)  break; 
			l.push_back( atof(cols[lCol].c_str()) ); 
			a.push_back( atof(cols[aCol].c_str()) ); 
			b.push_back( atof(cols[bCol].c_str()) ); 
		}
		in.close(); 

		if (! (l.size() == slicePos.size() && a.size() == slicePos.size() && b.size() == slicePos.size())) {
			AfxMessageBox("The number of rows in the CSV file does not match the number of tiles in the color checker.", 
				MB_ICONEXCLAMATION); 
			return; 
		}
		for (int i = 0; i < slicePos.size(); i++) {
			CSlice * slice = doc->GetSlice(slicePos[i]); 
			slice->setExpectedL(l[i]); 
			slice->setExpectedA(a[i]); 
			slice->setExpectedB(b[i]); 
		}
				
		OnOK(); 
	}
}

bool LABDialog::splitNextLine(istream& in, vector<string>& cols) 
{
	cols.clear(); 
	string line; 
	getline(in, line); 
	if (in.fail() || in.eof()) {	
		return false; 
	}
	istringstream lineIn(line); 
	string c; 
	while (getline(lineIn, c, ',')) {
		cols.push_back(c); 
	}
	return true; 
}

