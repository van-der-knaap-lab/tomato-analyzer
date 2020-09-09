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
// SaveSettings.cpp : implementation file
//

#include "stdafx.h"
#include "tomatoanalyzer.h"
#include "SaveSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSaveSettings dialog


CSaveSettings::CSaveSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CSaveSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSaveSettings)
	m_newusersave_str = _T("");
	//}}AFX_DATA_INIT
	isOK = false;
}


void CSaveSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSaveSettings)
	DDX_Control(pDX, IDC_EDIT1, m_newusersave);
	DDX_Control(pDX, IDC_LIST1, m_userboxsave);
	DDX_Text(pDX, IDC_EDIT1, m_newusersave_str);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSaveSettings, CDialog)
	//{{AFX_MSG_MAP(CSaveSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSaveSettings message handlers

void CSaveSettings::OnOK() 
{
	// TODO: Add extra validation here
	isOK = true;
	CDialog::OnOK();
}
