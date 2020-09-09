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
// MeasureItemList.cpp : implementation file
//

#include "stdafx.h"
#include "tomatoanalyzer.h"
#include "MeasureItemList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMeasureItemList

CMeasureItemList::CMeasureItemList()
{
    m_nCurColumn = -1;
}

CMeasureItemList::~CMeasureItemList()
{
}


BEGIN_MESSAGE_MAP(CMeasureItemList, CListCtrl)
	//{{AFX_MSG_MAP(CMeasureItemList)
	ON_WM_CREATE()
	//ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	//ON_NOTIFY_REFLECT_EX(LVN_COLUMNCLICK, OnColumnclick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMeasureItemList message handlers

void CMeasureItemList::DeleteAllColumns()
{
    int nRet;
    do
    {
        nRet = DeleteColumn(0);
    }
    while(nRet != 0);

    int nWidth = GetStringWidth("#") + 20;
    InsertColumn(0, "#",  LVCF_TEXT | LVCF_WIDTH | LVCFMT_LEFT, nWidth);
    return;
}

int CMeasureItemList::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	//LVS_EX_FULLROWSELECT might present a problem if trying to edit individual cells.
	//If taken out, only the first column has any effect when clicked upon
    SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

    LONG  oldStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE ); 
    SetWindowLong(m_hWnd, GWL_EXSTYLE, WS_EX_CLIENTEDGE | oldStyle);
	return 0;
}

void CMeasureItemList::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
    LPNMLVCUSTOMDRAW    pLvCd = (LPNMLVCUSTOMDRAW)pNMHDR;
	if(CDDS_PREPAINT == pLvCd->nmcd.dwDrawStage)
    {
        *pResult = CDRF_NOTIFYSUBITEMDRAW;
        return;
    }

    if(CDDS_ITEMPREPAINT == pLvCd->nmcd.dwDrawStage)
    {
        if(pLvCd->nmcd.dwItemSpec == m_nCurColumn)
        {
            *pResult = CDRF_NEWFONT;
            pLvCd->clrText = RGB(0, 0, 255);
        }
        else
        {
            *pResult = CDRF_DODEFAULT;
        }
        return;
    }

	return;
}

BOOL CMeasureItemList::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

    if(0 != pNMListView->iSubItem)
        m_nCurColumn = pNMListView->iSubItem;
	
	*pResult = 0;
    return FALSE;
}
