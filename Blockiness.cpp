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
// Blockiness.cpp : implementation file  
//

#include "stdafx.h"
#include "tomatoanalyzer.h"
#include "Blockiness.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBlockiness dialog


CBlockiness::CBlockiness(CWnd* pParent /*=NULL*/)
	: CDialog(CBlockiness::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBlockiness)
	m_fLwPos = 0.0f;
	m_fUpPos = 0.0f;
	//}}AFX_DATA_INIT
}


void CBlockiness::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBlockiness)
	DDX_Text(pDX, IDC_LOWER, m_fLwPos);
	DDX_Text(pDX, IDC_UPPER, m_fUpPos);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBlockiness, CDialog)
	//{{AFX_MSG_MAP(CBlockiness)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBlockiness message handlers

void CBlockiness::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}
