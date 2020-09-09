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
// ProgressDialog.cpp : implementation file
//

#include "stdafx.h"
#include "TomatoAnalyzer.h"
#include "ProgressDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressDialog dialog


CProgressDialog::CProgressDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressDialog)
	mMessage = _T("");
	//}}AFX_DATA_INIT
//	mProgressBar.SetRange(0, 100);
}


void CProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressDialog)
	DDX_Control(pDX, IDC_PROGRESS_CON, mProgressBar);
	DDX_Text(pDX, IDC_MESSAGE, mMessage);
	//}}AFX_DATA_MAP
}

void CProgressDialog::SetMessage(CString inMessage)
{
	mMessage = inMessage;
	UpdateData(FALSE);
}

void CProgressDialog::DoIncrement(float inMaxVal)
{
	float curPos = (float)mProgressBar.GetPos();
	curPos += mInc;
	if(curPos >= inMaxVal)
		mProgressBar.SetPos((int)inMaxVal);
	else
		mProgressBar.SetPos((int)curPos);
}


void CProgressDialog::SetIncrement(float inVal)
{
	if(inVal < 1.0f)
	{
		mInc = 1.0f;
	}
	else
		mInc = inVal;

}

void CProgressDialog::Update(short inPos)
{
	mProgressBar.SetPos(inPos);
}

void CProgressDialog::Update(short inPos, CString inMessage)
{
	mMessage = inMessage;
	mProgressBar.SetPos(inPos);
	UpdateData(FALSE);
}

BEGIN_MESSAGE_MAP(CProgressDialog, CDialog)
	//{{AFX_MSG_MAP(CProgressDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressDialog message handlers


BOOL CProgressDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	HICON theIcon;
	
	// TODO: Add extra initialization here
	mProgressBar.SetRange(0, 100);
	SetWindowText("Processing Progress");

	theIcon = AfxGetApp()->LoadIcon(IDI_SMALL_TOMATO);
	SetIcon(theIcon, true);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
