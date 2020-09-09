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
// Batch.cpp : implementation file
//

#include "stdafx.h"
#include "TomatoAnalyzer.h"
#include "Batch.h"
#include ".\batch.h"


// CBatch dialog

IMPLEMENT_DYNAMIC(CBatch, CDialog)
CBatch::CBatch(CWnd* pParent /*=NULL*/)
	: CDialog(CBatch::IDD, pParent)
	, m_radio_basic(0)
	, m_radio_std(1)
	, m_radio_indiv(2)
{
	m_radio_basic = 1;
	m_radio_std = 0;
	m_radio_indiv = 0;
}

CBatch::~CBatch()
{
}

void CBatch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColorTest)
	DDX_Check(pDX, IDC_BASIC, m_radio_basic);
	DDX_Check(pDX, IDC_STD, m_radio_std);
	DDX_Check(pDX, IDC_INDIV, m_radio_indiv);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBatch, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CColorTest message handlers

void CBatch::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
