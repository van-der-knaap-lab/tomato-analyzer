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
// LoadSettings1.cpp : implementation file
//

#include "stdafx.h"
#include "tomatoanalyzer.h"
#include "LoadSettings1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoadSettings dialog


CLoadSettings::CLoadSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CLoadSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoadSettings)
	m_userslist = _T("");
	//}}AFX_DATA_INIT
	//CString user_cstr = GetProfileString("Settins", "Users", "");
	//SetArray((LPCTSTR)user_cstr);


}

CLoadSettings::CLoadSettings(std::string userlist, CWnd* pParent /*=NULL*/)
	: CDialog(CLoadSettings::IDD, pParent)
{
	
	//{{AFX_DATA_INIT(CLoadSettings)
	m_userslist = _T("");
	user_str = userlist;
	//}}AFX_DATA_INIT

}


int CLoadSettings::DoModal(std::string userlist){
	user_str = userlist;
	return CDialog::DoModal();
}

// add entries to listbox
BOOL CLoadSettings::OnInitDialog() {
    CDialog::OnInitDialog();

	//CString user_cstr = CTomatoAnalyzerDoc::GetProfileString("Settins", "Users", "");
	//std::string user_str((LPCTSTR)user_cstr);


	CListBox *users = (CListBox*)GetDlgItem(IDC_LIST1);
	while(user_str.find("|",1) != std::string::npos){
		std::string item_str = user_str.substr(0, user_str.find("|",1));
		user_str = user_str.substr(user_str.find("|",1));
		users->AddString(item_str.c_str());
	}

    /*CListBox *teams = (CListBox*)GetDlgItem(IDC_LIST1);
    teams->AddString("broncos");
    teams->AddString("Packers");*/
    return TRUE;
}

/*
void CLoadSettings::SetArray(std::string user_str){
CListBox *users = (CListBox*)GetDlgItem(IDC_LIST1);
	while(user_str.find("|",0)){
		std::string item_str = user_str.substr(0, user_str.find("|",0));
		user_str = user_str.substr(user_str.find("|",0));
		users->AddString(item_str.c_str());
	}
}*/

void CLoadSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoadSettings)
	DDX_Control(pDX, IDC_LIST1, m_userlistbox);
	DDX_LBString(pDX, IDC_LIST1, m_userslist);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoadSettings, CDialog)
	//{{AFX_MSG_MAP(CLoadSettings)
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeList1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoadSettings message handlers

void CLoadSettings::OnSelchangeList1() 
{
	// TODO: Add your control notification handler code here
		CString str = _T("");
	for( int j=0; j<10; j++ )
    {
        str.Format( "Item %d", j+1 );
        int idx = m_userlistbox.AddString( str );
        m_userlistbox.SetItemData( idx, j+1 );
    }
}
