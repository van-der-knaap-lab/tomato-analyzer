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
// SelectMeasurement.cpp : implementation file
//

#include "stdafx.h"
#include "tomatoanalyzer.h"
#include "CheckboxTreeCtrl.h"
#include "SelectMeasurement.h"
#include "MeasurementSet.h"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectMeasurement dialog


CSelectMeasurement::CSelectMeasurement(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectMeasurement::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectMeasurement)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSelectMeasurement::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectMeasurement)
	DDX_Control(pDX, IDC_TREE1, m_ctrTree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectMeasurement, CDialog)
	//{{AFX_MSG_MAP(CSelectMeasurement)
	ON_BN_CLICKED(IDC_SELECT_ALL, OnSelectAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectMeasurement message handlers

void CSelectMeasurement::OnOK() 
{
	// TODO: Add extra validation here
	m_vctSelectedItem.clear();
    m_ctrTree.RetrieveCheckedItemID(m_vctSelectedItem);

	CDialog::OnOK();
}

BOOL CSelectMeasurement::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
    MeasurementSet measurementSet = ((CTomatoAnalyzerApp *)AfxGetApp())->getMeasurementSet(); 
    int nGroupNumber = measurementSet.numGroups();
    HTREEITEM * pItemHandle = new HTREEITEM[nGroupNumber];

	TVINSERTSTRUCT     TreeItem;
    TreeItem.hParent = TVI_ROOT;
    TreeItem.hInsertAfter = TVI_LAST;
    TreeItem.item.mask = TVIF_TEXT | TVIF_PARAM;
    for(int i=0; i<nGroupNumber; i++)
    {
		CString name = measurementSet.groupName(i); 
        TreeItem.item.lParam  = i;
        TreeItem.item.pszText = name.GetBuffer(name.GetLength()); 
        pItemHandle[i] = m_ctrTree.InsertItem(&TreeItem);
    }

    int nMeasureNumber = measurementSet.numMeasurements(); 
    for(int i=0; i<nMeasureNumber; i++)
    {
		int nParentID = measurementSet.groupForMeasurement(i); 
		CString name = measurementSet.measurementName(i); 

        TreeItem.item.pszText = name.GetBuffer(name.GetLength()); 
        TreeItem.hParent = pItemHandle[nParentID];
        TreeItem.item.lParam = i; 
		
        HTREEITEM  hItem = m_ctrTree.InsertItem(&TreeItem);
    }

    m_ctrTree.SetCheckboxStatus(m_vctSelectedItem);

    delete [] pItemHandle;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectMeasurement::OnSelectAll() 
{
	// TODO: Add your control notification handler code here
	m_ctrTree.SetCheckboxForAllItems();
    return;
}
