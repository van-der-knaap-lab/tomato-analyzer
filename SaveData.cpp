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
// SaveData.cpp : implementation file
//

#include "stdafx.h"
#include "tomatoanalyzer.h"
#include "SaveData.h"

#include <vector>
#include <string>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern char * getMeasurementName(int i, int& nMethod);
extern int getMeasurementNumber(void);


/////////////////////////////////////////////////////////////////////////////
// CSaveData dialog


CSaveData::CSaveData(CWnd* pParent /*=NULL*/)
	: CDialog(CSaveData::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSaveData)
	//}}AFX_DATA_INIT
}


void CSaveData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSaveData)
	DDX_Control(pDX, IDC_Candidates, m_Candidates);
	DDX_Control(pDX, IDC_Selected, m_Selected);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSaveData, CDialog)
	//{{AFX_MSG_MAP(CSaveData)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_LBN_DBLCLK(IDC_Candidates, OnDblclkCandidates)
	ON_LBN_DBLCLK(IDC_Selected, OnDblclkSelected)
	ON_BN_CLICKED(IDC_SAVEDATA_UP, OnSavedataUp)
	ON_BN_CLICKED(IDC_SAVEDATA_DOWN, OnSavedataDown)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnSelchangeTab1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSaveData message handlers

BOOL CSaveData::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
    int nMethod, iIndex, iLabel;
    int nNum = getMeasurementNumber();
    
    char szBuffer[64];
    std::vector<int>::iterator  ipos, bgn, ed;
    bgn = m_vctSelectedItem.begin();
    ed  = m_vctSelectedItem.end();
    while(bgn != ed)
    {
        int ntemp;
        char * szAttribute = getMeasurementName((*bgn)/10, ntemp);
        wsprintf(szBuffer, "%s(%d)", szAttribute, (*bgn)%10);
        iIndex = m_Selected.AddString(szBuffer);
        m_Selected.SetItemData(iIndex, *bgn);


        ++bgn;
    }
    bgn = m_vctSelectedItem.begin();

    for(int i=0; i<nNum; i++)
    {
        char * szAttribute = getMeasurementName(i, nMethod);

        for(int k=1; k<=nMethod; k++)
        {
            wsprintf(szBuffer, "%s(%d)", szAttribute, k);

            iLabel = i*10 + k;
            ipos = std::find(bgn, ed, iLabel);
            if(ipos == ed)
            {
                iIndex = m_Candidates.AddString(szBuffer);
                m_Candidates.SetItemData(iIndex, iLabel);
            }
        }
    }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CSaveData::OnAdd() 
{
	// TODO: Add your control notification handler code here
    AddRemoveItem(m_Selected, m_Candidates);
    return;
}

void CSaveData::OnRemove() 
{
	// TODO: Add your control notification handler code here
	AddRemoveItem(m_Candidates, m_Selected);
    return;
}

void CSaveData::AddRemoveItem(CListBox& lstadd, CListBox& lstremove)
{
    int iSelected = lstremove.GetCurSel();
    if(LB_ERR == iSelected)
    {
        AfxMessageBox("Choose one item first, please", MB_ICONINFORMATION|MB_OK);
        return;
    }

    int nMethod;
    char szBuffer[64];
    int nLabel = lstremove.GetItemData(iSelected);
    int iItem = nLabel/10;
    lstremove.DeleteString(iSelected);

    char * szAttribute = getMeasurementName(iItem, nMethod);
    wsprintf(szBuffer, "%s(%d)", szAttribute, nLabel%10);
    int iIndex = lstadd.AddString(szBuffer);
    lstadd.SetItemData(iIndex, nLabel);
 
    return;
}


void CSaveData::OnDblclkCandidates() 
{
	// TODO: Add your control notification handler code here
    AddRemoveItem(m_Selected, m_Candidates);
}



void CSaveData::OnDblclkSelected() 
{
	// TODO: Add your control notification handler code here
    AddRemoveItem(m_Candidates, m_Selected);

}

void CSaveData::OnSavedataUp() 
{
	// TODO: Add your control notification handler code here
    SequenceAdjust(-1);	
}

void CSaveData::OnSavedataDown() 
{
	// TODO: Add your control notification handler code here
    SequenceAdjust(1);
	
}


void CSaveData::SequenceAdjust(int nDir)
{
    int iSelected = m_Selected.GetCurSel();
    if(LB_ERR == iSelected)
    {
        AfxMessageBox("Choose one item first, please", MB_ICONINFORMATION|MB_OK);
        return;
    }

    char szBuffer[64];
    m_Selected.GetText(iSelected, szBuffer);
    int nLabel = m_Selected.GetItemData(iSelected);
    
    int iChangedTo = iSelected + nDir;
    int nCount = m_Selected.GetCount();
    if(iChangedTo<0)
        iChangedTo = nCount-1;
    else if(iChangedTo>=nCount)
        iChangedTo = 0;

    m_Selected.DeleteString(iSelected);
    m_Selected.InsertString(iChangedTo, szBuffer);
    m_Selected.SetItemData(iChangedTo, nLabel);
    m_Selected.SetCurSel(iChangedTo);

    return;
}

void CSaveData::OnOK() 
{
	// TODO: Add extra validation here
    m_vctSelectedItem.clear();

    int nCount = m_Selected.GetCount();
    for(int i=0; i<nCount; i++)
    {
        int iData = m_Selected.GetItemData(i);
        m_vctSelectedItem.push_back(iData);
    }

	CDialog::OnOK();
    return;
}

void CSaveData::OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}
