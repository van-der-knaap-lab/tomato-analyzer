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
// OldTmtDialog.cpp : implementation file
//

#include "stdafx.h"
#include "TomatoAnalyzer.h"
#include "OldTmtDialog.h"
#include "afxdialogex.h"


// OldTmtDialog dialog

IMPLEMENT_DYNAMIC(OldTmtDialog, CDialog)

OldTmtDialog::OldTmtDialog(CWnd* pParent /*=NULL*/)
	: CDialog(OldTmtDialog::IDD, pParent)
{

}

OldTmtDialog::~OldTmtDialog()
{
}

void OldTmtDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(OldTmtDialog, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// OldTmtDialog message handlers
void OldTmtDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}