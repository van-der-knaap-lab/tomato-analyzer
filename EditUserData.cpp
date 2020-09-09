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
// EditUserData.cpp : implementation file
//

#include "stdafx.h"
#include "TomatoAnalyzer.h"
#include "EditUserData.h"


// EditUserData dialog

IMPLEMENT_DYNAMIC(EditUserData, CDialog)

EditUserData::EditUserData(CWnd* pParent /*=NULL*/)
	: CDialog(EditUserData::IDD, pParent)
{

}

EditUserData::EditUserData(CWnd* pParent, std::vector<CString> columns, int numCols):CDialog(EditUserData::IDD, pParent)
{
	CString textEdit[30];
	for(int i = 0; i < 30; i++)
	{
		textEdit[i] = "";
	}
	textEditCopy = columns;
	loopControl = numCols;
	CEdit editBoxes[30];
}

EditUserData::~EditUserData()
{
}

void EditUserData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATACOL1, editBoxes[0]);
	DDX_Control(pDX, IDC_DATACOL2, editBoxes[1]);
	DDX_Control(pDX, IDC_DATACOL3, editBoxes[2]);
	DDX_Control(pDX, IDC_DATACOL4, editBoxes[3]);
	DDX_Control(pDX, IDC_DATACOL5, editBoxes[4]);
	DDX_Control(pDX, IDC_DATACOL6, editBoxes[5]);
	DDX_Control(pDX, IDC_DATACOL7, editBoxes[6]);
	DDX_Control(pDX, IDC_DATACOL8, editBoxes[7]);
	DDX_Control(pDX, IDC_DATACOL9, editBoxes[8]);
	DDX_Control(pDX, IDC_DATACOL10, editBoxes[9]);
	DDX_Control(pDX, IDC_DATACOL11, editBoxes[10]);
	DDX_Control(pDX, IDC_DATACOL12, editBoxes[11]);
	DDX_Control(pDX, IDC_DATACOL13, editBoxes[12]);
	DDX_Control(pDX, IDC_DATACOL14, editBoxes[13]);
	DDX_Control(pDX, IDC_DATACOL15, editBoxes[14]);
	DDX_Control(pDX, IDC_DATACOL16, editBoxes[15]);
	DDX_Control(pDX, IDC_DATACOL17, editBoxes[16]);
	DDX_Control(pDX, IDC_DATACOL18, editBoxes[17]);
	DDX_Control(pDX, IDC_DATACOL19, editBoxes[18]);
	DDX_Control(pDX, IDC_DATACOL20, editBoxes[19]);
	DDX_Control(pDX, IDC_DATACOL21, editBoxes[20]);
	DDX_Control(pDX, IDC_DATACOL22, editBoxes[21]);
	DDX_Control(pDX, IDC_DATACOL23, editBoxes[22]);
	DDX_Control(pDX, IDC_DATACOL24, editBoxes[23]);
	DDX_Control(pDX, IDC_DATACOL25, editBoxes[24]);
	DDX_Control(pDX, IDC_DATACOL26, editBoxes[25]);
	DDX_Control(pDX, IDC_DATACOL27, editBoxes[26]);
	DDX_Control(pDX, IDC_DATACOL28, editBoxes[27]);
	DDX_Control(pDX, IDC_DATACOL29, editBoxes[28]);
	DDX_Control(pDX, IDC_DATACOL30, editBoxes[29]);
	DDX_Text(pDX, IDC_DATACOL1, textEdit[0]);
	DDX_Text(pDX, IDC_DATACOL2, textEdit[1]);
	DDX_Text(pDX, IDC_DATACOL3, textEdit[2]);
	DDX_Text(pDX, IDC_DATACOL4, textEdit[3]);
	DDX_Text(pDX, IDC_DATACOL5, textEdit[4]);
	DDX_Text(pDX, IDC_DATACOL6, textEdit[5]);
	DDX_Text(pDX, IDC_DATACOL7, textEdit[6]);
	DDX_Text(pDX, IDC_DATACOL8, textEdit[7]);
	DDX_Text(pDX, IDC_DATACOL9, textEdit[8]);
	DDX_Text(pDX, IDC_DATACOL10, textEdit[9]);
	DDX_Text(pDX, IDC_DATACOL11, textEdit[10]);
	DDX_Text(pDX, IDC_DATACOL12, textEdit[11]);
	DDX_Text(pDX, IDC_DATACOL13, textEdit[12]);
	DDX_Text(pDX, IDC_DATACOL14, textEdit[13]);
	DDX_Text(pDX, IDC_DATACOL15, textEdit[14]);
	DDX_Text(pDX, IDC_DATACOL16, textEdit[15]);
	DDX_Text(pDX, IDC_DATACOL17, textEdit[16]);
	DDX_Text(pDX, IDC_DATACOL18, textEdit[17]);
	DDX_Text(pDX, IDC_DATACOL19, textEdit[18]);
	DDX_Text(pDX, IDC_DATACOL20, textEdit[19]);
	DDX_Text(pDX, IDC_DATACOL21, textEdit[20]);
	DDX_Text(pDX, IDC_DATACOL22, textEdit[21]);
	DDX_Text(pDX, IDC_DATACOL23, textEdit[22]);
	DDX_Text(pDX, IDC_DATACOL24, textEdit[23]);
	DDX_Text(pDX, IDC_DATACOL25, textEdit[24]);
	DDX_Text(pDX, IDC_DATACOL26, textEdit[25]);
	DDX_Text(pDX, IDC_DATACOL27, textEdit[26]);
	DDX_Text(pDX, IDC_DATACOL28, textEdit[27]);
	DDX_Text(pDX, IDC_DATACOL29, textEdit[28]);
	DDX_Text(pDX, IDC_DATACOL30, textEdit[29]);
}


BEGIN_MESSAGE_MAP(EditUserData, CDialog)
	ON_BN_CLICKED(IDOK, &EditUserData::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &EditUserData::OnBnClickedCancel)
END_MESSAGE_MAP()


BOOL EditUserData::OnInitDialog()
{
	CDialog::OnInitDialog();

	for(int i = 0; i < loopControl; i++)
	{
		textEdit[i] = textEditCopy[i];
		editBoxes[i].ShowWindow(TRUE);
		editBoxes[i].SetWindowTextA(textEdit[i]);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// EditUserData message handlers

void EditUserData::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void EditUserData::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
