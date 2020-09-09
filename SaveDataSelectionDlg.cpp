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
// SaveDataSelectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tomatoanalyzer.h"
#include "CheckboxTreeCtrl.h"
#include "SaveDataSelectionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*extern int GetGroupNumber();
extern int GetTotalMeasurementNumber();
extern int GetMeasurementID(int nMeasurementID, int nMethod);
extern LPSTR  GetGroupName(int iIndex);
extern LPSTR  GetMeasurementName(int iIndex, int& nParentID, int& nMethodNumber);*/

/////////////////////////////////////////////////////////////////////////////
// CSaveDataSelectionDlg dialog


CSaveDataSelectionDlg::CSaveDataSelectionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSaveDataSelectionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSaveDataSelectionDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSaveDataSelectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSaveDataSelectionDlg)
	DDX_Control(pDX, IDC_SELECTION_TREE, m_ctrTree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSaveDataSelectionDlg, CDialog)
	//{{AFX_MSG_MAP(CSaveDataSelectionDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSaveDataSelectionDlg message handlers

BOOL CSaveDataSelectionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();


	
	// TODO: Add extra initialization here
    //int nGroupNumber = GetGroupNumber();
	MeasurementSet measurementSet = ((CTomatoAnalyzerApp *)AfxGetApp())->getMeasurementSet(); 
	
	int nGroupNumber = measurementSet.numGroups();
    HTREEITEM * pItemHandle = new HTREEITEM[nGroupNumber];

	TVINSERTSTRUCT     TreeItem;
    TreeItem.hParent = TVI_ROOT;
    TreeItem.hInsertAfter = TVI_LAST;
    TreeItem.item.mask = TVIF_TEXT | TVIF_PARAM;
    for(int i=0; i<nGroupNumber; i++)
    {
        TreeItem.item.lParam  = i;
        CString groupName = measurementSet.groupName(i); 
		TreeItem.item.pszText = groupName.GetBuffer(groupName.GetLength());
        pItemHandle[i] = m_ctrTree.InsertItem(&TreeItem);
    }

    int nMeasureNumber = measurementSet.numMeasurements(); 
    for(int i=0; i<nMeasureNumber; i++)
    {
        CString measurementName = measurementSet.measurementName(i); 
		TreeItem.item.pszText = measurementName.GetBuffer(measurementName.GetLength()); 
		int nParentID = measurementSet.groupForMeasurement(i);
        TreeItem.hParent = pItemHandle[nParentID];
        TreeItem.item.lParam = i;
		HTREEITEM hItem = m_ctrTree.InsertItem(&TreeItem);
    }

    delete [] pItemHandle;
    
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CSaveDataSelectionDlg::OnSelchangedSelectionTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CSaveDataSelectionDlg::OnClickSelectionTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
    POINT   pt;
    TVHITTESTINFO    hitInfo;

    GetCursorPos(&pt);
    m_ctrTree.ScreenToClient(&pt);
    hitInfo.pt = pt;
    HTREEITEM  hItem = m_ctrTree.HitTest(&hitInfo);

    if(NULL != hItem)
    {
        if((TVHT_ONITEMSTATEICON  & hitInfo.flags) == TVHT_ONITEMSTATEICON )
        {
            m_ctrTree.SelectItem(hItem);
          
            UpdateSelectionStatus(hItem);
        }
    }

//	m_ctrTree.SelectItem(pNMHDR->hwndFrom);
	*pResult = 0;
}


void CSaveDataSelectionDlg::UpdateAllChildren(HTREEITEM hItem, UINT bChecked)
{
    TVITEM  tvItem;
    tvItem.mask = TVIF_HANDLE | TVIF_STATE;
    tvItem.state = INDEXTOSTATEIMAGEMASK(bChecked);
    tvItem.stateMask = TVIS_STATEIMAGEMASK;

    HTREEITEM   hChild = m_ctrTree.GetChildItem(hItem);
    while(hChild)
    {
        tvItem.hItem = hChild;
        m_ctrTree.SetItem(&tvItem);
        
        if(m_ctrTree.ItemHasChildren(hChild))
            UpdateAllChildren(hChild, bChecked);            

        hChild = m_ctrTree.GetNextItem(hChild, TVGN_NEXT);
    }

    return;
}

void CSaveDataSelectionDlg::UpdateUpdateParents(HTREEITEM hItem, UINT bChecked)
{
    HTREEITEM hItemParent = m_ctrTree.GetParentItem(hItem);
    if(TVI_ROOT == hItemParent)
        return;

    if(allSelected(hItemParent, bChecked))
    {
        m_ctrTree.SetItemState(hItemParent, bChecked, TVIS_STATEIMAGEMASK);
        UpdateUpdateParents(hItemParent, bChecked);
    }

    return;
}


void CSaveDataSelectionDlg::UpdateSelectionStatus(HTREEITEM hItem)
{
    UINT uState = m_ctrTree.GetItemState(hItem, TVIS_STATEIMAGEMASK);
    UINT bChecked = ((uState>>12)==1)? 2 : 1;


    if(m_ctrTree.ItemHasChildren(hItem))
    {
        UpdateAllChildren(hItem, bChecked);
        UpdateUpdateParents(hItem, bChecked);
    }
    else
    {
//        UpdateUpdateParents(hItem, bChecked);
    }

    return;


}

BOOL CSaveDataSelectionDlg::allSelected(HTREEITEM hItem, UINT sta)
{
    HTREEITEM hChild = m_ctrTree.GetChildItem(hItem);
    while(hChild)
    {
        UINT uState = m_ctrTree.GetItemState(hChild, TVIS_STATEIMAGEMASK);
        UINT bChecked = (uState>>12);
        if(bChecked != sta)
            return FALSE;

        hChild = m_ctrTree.GetNextItem(hChild, TVGN_NEXT);
    }

    return TRUE;
}

