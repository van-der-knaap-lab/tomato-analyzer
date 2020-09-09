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
This file is part of Tomato Analyzer.

    Tomato Analyzer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Tomato Analyzer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Tomato Analyzer.  If not, see <http://www.gnu.org/licenses/>.
// LoadSettings.cpp : implementation file
//

#include "stdafx.h"
#include "TomatoAnalyzer.h"
#include "LoadSettings.h"
#include ".\loadsettings.h"


// CLoadSettings dialog

IMPLEMENT_DYNAMIC(CLoadSettings, CDialog)
CLoadSettings::CLoadSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CLoadSettings::IDD, pParent)
{
	/*UserListBox.AddString("Hello");
	UserListBox.AddString("My");
	UserListBox.AddString("Name");*/
}

CLoadSettings::~CLoadSettings()
{
}

BOOL CLoadSettings::OnInitDialog()
{
	UserListBox.AddString("Hello");
	UserListBox.AddString("My");
	UserListBox.AddString("Name");
	return 1;
}

void CLoadSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, UserListBox);
}


BEGIN_MESSAGE_MAP(CLoadSettings, CDialog)
	ON_BN_CLICKED(IDOK, OnOK)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
END_MESSAGE_MAP()

// CLoadSettings message handlers

void CLoadSettings::OnOK()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CLoadSettings::OnCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
