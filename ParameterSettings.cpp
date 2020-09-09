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
// ParameterSettings.cpp : implementation file
//

#include "stdafx.h"
#include "tomatoanalyzer.h"
#include "ParameterSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CParameterSettings dialog


CParameterSettings::CParameterSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CParameterSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CParameterSettings)
	m_ShapeIndex = 1;
	m_shapeThreshold = 0.0;
	//}}AFX_DATA_INIT
}


void CParameterSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParameterSettings)
	DDX_Text(pDX, IDC_SHAPEINDEX, m_ShapeIndex);
	DDV_MinMaxUInt(pDX, m_ShapeIndex, 1, 99);
	DDX_Text(pDX, IDC_CURVE_THRESHOLD, m_shapeThreshold);
	DDV_MinMaxDouble(pDX, m_shapeThreshold, 0., 6.);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParameterSettings, CDialog)
	//{{AFX_MSG_MAP(CParameterSettings)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParameterSettings message handlers

VOID CParameterSettings::GetParameter(int &nShapeIndex)
{
	nShapeIndex=m_ShapeIndex;
}


VOID CParameterSettings::SetParameters(int nShapeIndex)
{
	m_ShapeIndex = nShapeIndex;
}
