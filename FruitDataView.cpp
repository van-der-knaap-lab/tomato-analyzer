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
// FruitDataView.cpp : implementation file
//

#include "stdafx.h"
#include "BoundingBoxList.h"
#include "Slice.h"
#include "TomatoAnalyzerDoc.h"
#include "tomatoanalyzer.h"
#include "MeasureItemList.h"
#include "FruitDataView.h"
#include <fstream>
#include <algorithm>
#include "MeasurementSet.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFruitDataView

IMPLEMENT_DYNCREATE(CFruitDataView, CView)

CFruitDataView::CFruitDataView()
{
    m_vctGroupItem.reserve(256);
}

CFruitDataView::~CFruitDataView()
{
}


BEGIN_MESSAGE_MAP(CFruitDataView, CView)
	//{{AFX_MSG_MAP(CFruitDataView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_PAGE, OnGroupTabChanged)
    ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST2, OnCurMeasurementChanged)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST2, OnItemChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFruitDataView drawing

void CFruitDataView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CFruitDataView diagnostics

#ifdef _DEBUG
void CFruitDataView::AssertValid() const
{
	CView::AssertValid();
}

void CFruitDataView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTomatoAnalyzerDoc* CFruitDataView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTomatoAnalyzerDoc)));
	return (CTomatoAnalyzerDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFruitDataView message handlers

int CFruitDataView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
    RECT    rect;
    GetClientRect(&rect);

	m_page.Create(TCS_MULTILINE | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | WS_TABSTOP | WS_BORDER, 
                  rect, 
                  this, 
                  IDC_PAGE
                 );

    rect.right  = CW_USEDEFAULT;
    rect.bottom = CW_USEDEFAULT;
	//LVS_REPORT mode keeping only the first column selectable?  LVS_EDITLABELS seems to allow editing the columns but only the first
	//column can be selected if full row selection is turned off in MeasureItemList.cpp.
    m_ListData.Create(LVS_REPORT|LVS_SINGLESEL|LVS_SHOWSELALWAYS|WS_CHILD|WS_TABSTOP|WS_VISIBLE|WS_BORDER|WS_HSCROLL|WS_VSCROLL,
                      rect,
                      this,
                      IDC_LIST2
                     );

    m_ImageList.Create(IDB_LEGEND, 15, 1, RGB(255, 255, 255));

    CHeaderCtrl *   pControl = m_ListData.GetHeaderCtrl();
    CImageList*  P = pControl->SetImageList(&m_ImageList);
    pControl->SendMessage(HDM_FIRST + 20, 3, 0);

    OnMeasurementChange();
    return 0;
}


void CFruitDataView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
    RECT    rect;
    SetRect(&rect, 0, 0, cx, cy); 
    m_page.AdjustRect(FALSE, &rect); 
 
    HDWP hdwp = BeginDeferWindowPos(2); 
    DeferWindowPos(hdwp, 
                   m_page.m_hWnd, 
                   NULL, 
                   0, 0, cx, cy,
                   SWP_NOMOVE | SWP_NOZORDER 
                   );
    rect.top += 10;
    DeferWindowPos(hdwp, 
                   m_ListData.m_hWnd, 
                   HWND_TOP, 
                   rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, 
                   SWP_SHOWWINDOW 
                  ); 
    EndDeferWindowPos(hdwp); 

}

void CFruitDataView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
}

/**
 * This is called automatically when CTomatoAnalyzerDoc::UpdateAllViews is called. 
 * If the correct LPARAM is passed to that function, then it will be passed to lHint
 * and this function will update the tab set, column set, or selected row(?). Otherwise it
 * will just update the data shown. 
 */
void CFruitDataView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
    switch(lHint)
    {
    case DOC_UPDATEVIEW_FRUITSWITCH:
         OnCurFruitChanged();
         return;

    case DOC_UPDATEVIEW_MEASURECHANGED:
         OnMeasurementChange();
         {
             RECT   rect;
             GetClientRect(&rect);

             OnSize(SIZE_RESTORED, rect.right, rect.bottom);
         }
         break;

    case DOC_UPDATEVIEW_FRUITESTATE:
         break;
    }
    ShowMeasurementValues();
}

void CFruitDataView::BuildReportItems()
{
	CTomatoAnalyzerDoc *    pDoc = GetDocument();

	std::vector<POSITION>   vctPos;
	pDoc->GetSelectedSliceByNaturalSequence(vctPos);

	char    szBuffer[256];
	int nSelectedNumber = vctPos.size();
	for(int i=0; i<nSelectedNumber; i++)
	{
		wsprintf(szBuffer, "%02d", i+1);
		m_ListData.InsertItem(i, szBuffer);
	}
}

/**
 * In the report, creates an empty column with header for each currently selected measurement. 
 */ 
void CFruitDataView::BuildReportColumns()
{
	m_ListData.DeleteAllColumns();

	m_vctGroupItem.clear();

	int  nSelectedGroup = GetCurMeasurementGroup();
	if(-1 == nSelectedGroup)
		return;

	CHeaderCtrl *   pHdrCtrl = m_ListData.GetHeaderCtrl();

	MeasurementSet allMeas = ((CTomatoAnalyzerApp *)AfxGetApp())->getMeasurementSet();
	int iColum = 1;

	// The order in which columns appear within a tab is determined by allMeas, not by m_vctSelectedItem. 
	vector<CString> groupMeas = allMeas.measurementsInGroup(allMeas.groupName(nSelectedGroup)); 
	for (vector<CString>::iterator i = groupMeas.begin(); i != groupMeas.end(); i++) 
	{
		CString measName = *i; 
		int nItemIndex = allMeas.measurementID(measName); 
		if (std::find(m_vctSelectedItem.begin(), m_vctSelectedItem.end(), nItemIndex) != m_vctSelectedItem.end()) 
		{
			LPSTR lpName = measName.GetBuffer(measName.GetLength()); 
			int nParentID = allMeas.groupForMeasurement(nItemIndex); 

			if(! measName.IsEmpty() && nParentID==nSelectedGroup) 
			{
				int nWidth = m_ListData.GetStringWidth(lpName) + 35;

				LVCOLUMN    lvc;
				lvc.mask  = LVCF_TEXT | LVCF_WIDTH;
				lvc.cx    = nWidth;
				lvc.pszText = lpName;
				m_ListData.InsertColumn(iColum, &lvc);

				HDITEM  curItem;
				memset(&curItem, 0, sizeof(curItem));
				curItem.mask   = HDI_FORMAT | HDI_IMAGE | HDI_LPARAM ;
				curItem.fmt    = HDF_LEFT | HDF_IMAGE | HDF_STRING  ;
				curItem.iImage = nItemIndex;
				curItem.lParam = nItemIndex;
				BOOL b = pHdrCtrl->SetItem(iColum, &curItem);

				iColum++;

				m_vctGroupItem.push_back(nItemIndex);
			}
		}
	}

}

void CFruitDataView::Test()
{
    CHeaderCtrl *   pHeader = m_ListData.GetHeaderCtrl();

    char    szBuffer[256];
    HDITEM  hdi;
    int n = pHeader->GetItemCount();
    for(int i=0; i<n; i++)
    {
        memset(szBuffer, 0, sizeof(szBuffer));
        memset(&hdi, 0, sizeof(hdi));
        hdi.mask = (UINT) (-1);
        hdi.pszText = szBuffer;
        hdi.cchTextMax = sizeof(szBuffer)-1;
    
        pHeader->GetItem(i, &hdi);
    }
}

/**
 * Builds or rebuilds the set of tabs in the report. 
 */
void CFruitDataView::OnMeasurementChange()
{
    m_vctSelectedItem = ((CTomatoAnalyzerApp *)AfxGetApp())->m_vctSaveDataItem;

	m_page.DeleteAllItems();

    MeasurementSet measurementSet = ((CTomatoAnalyzerApp *)AfxGetApp())->getMeasurementSet(); 
    int nGroupNumber = measurementSet.numGroups(); 
	std::vector<bool> nGroupUsed(nGroupNumber, false); 
    
	for (std::vector<int>::iterator Iter = m_vctSelectedItem.begin(); Iter != m_vctSelectedItem.end(); Iter++) 
    {
        int nItemIndex = *Iter; 

			CString measName = measurementSet.measurementName(nItemIndex); 
			LPSTR lpName = measName.GetBuffer(measName.GetLength()); 
			int nParentID = measurementSet.groupForMeasurement(nItemIndex); 

			if (! measName.IsEmpty())
                nGroupUsed[nParentID] = true;
    }

    for(int i=0; i<nGroupNumber; i++)
    {
        if(nGroupUsed[i])
        {
			CString groupName = measurementSet.groupName(i); 

            TCITEM tie;
            
            tie.mask    = TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM; 
            tie.iImage  = -1;
            tie.lParam  = i;
            tie.pszText = groupName.GetBuffer(groupName.GetLength());
            m_page.InsertItem(i, &tie);
        }
    }

    m_page.SetCurSel(0);

    LONG    lResult;
    OnGroupTabChanged(NULL, &lResult);
}

int  CFruitDataView::GetCurMeasurementGroup()
{
    int  nSelectedItem = m_page.GetCurSel();
    if(-1 == nSelectedItem)
        return -1;

    TCITEM tie;
    tie.mask = TCIF_PARAM;
    m_page.GetItem(nSelectedItem, &tie);
    
    return tie.lParam;

}

/// This method is called everytime a tab is clicked.
void CFruitDataView::OnGroupTabChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	BuildReportColumns();
	ShowMeasurementValues();
	*pResult = 0;
}

/// This method displays all data in the report for selected fruits 
void CFruitDataView::ShowMeasurementValues()
{
	m_ListData.DeleteAllItems();

	int nMeasureGroup = GetCurMeasurementGroup();
	if(-1 == nMeasureGroup)
		return;

	m_ListData.SetRedraw(FALSE);

	CTomatoAnalyzerDoc *    pDoc = GetDocument();
	POSITION    posCurSelected = pDoc->GetSelectedObject();

	std::vector<POSITION>   vctPos;
	pDoc->GetSelectedSliceByNaturalSequence(vctPos);

	int nSelectedSlices = vctPos.size();
	
	std::vector<POSITION>::iterator  Iter = vctPos.begin();
	m_ListData.SetItemCount(nSelectedSlices);

	 // If the tab is the User Created Tab, then do the following
	 MeasurementSet measurementSet = ((CTomatoAnalyzerApp*)AfxGetApp())->getMeasurementSet(); 
	 if (nMeasureGroup == measurementSet.groupID("User Created"))
	 {
		 // Grabs the list of column names in the User Created tab
		 std::vector<CString> measInGroup = measurementSet.measurementsInGroup("User Created");
		 // Loops over the number of slices that are selected 
		 for(int i = 0; i < nSelectedSlices; i++)
		 {
			 char    szBuffer[256];
			 wsprintf(szBuffer, "%02d", i+1);

			  // Creates objects in order to get the indices of the selected slices
			 CTomatoAnalyzerDoc userObject;
			 CSlice   *Slice = userObject.GetSlice(*Iter);
			 // Sets the numbers that list the fruits
			 LVITEM  lvi;
			 memset(&lvi, 0, sizeof(LVITEM));
			 lvi.mask    = LVIF_TEXT | LVIF_PARAM ;
			 lvi.iItem   = i;
			 lvi.pszText = szBuffer;
			 lvi.lParam  = (LONG)(*Iter);
			 m_ListData.InsertItem(&lvi);

			 // Creates objects in order to get the indices of the selected slices
			 CSlice * Slices = pDoc->GetSlice(*Iter);
			 // Places the corresponding data into the report window
			 for (int j = 0; j < measInGroup.size(); j++) {
				 m_ListData.SetItemText(i, j+1, Slices->getUserCreatedData(measInGroup[j])); 
			 }
			 ++Iter;
		 }
		
	 }
	 else // If the tab is anything else, do these:
	 {
		char    szBuffer[256];
		for(int i=0; i<nSelectedSlices; i++)
		{    
	
				//Convert to a string
				wsprintf(szBuffer, "%02d", i+1);
		
				LVITEM  lvi;
				memset(&lvi, 0, sizeof(LVITEM));
				lvi.mask    = LVIF_TEXT | LVIF_PARAM ;
				lvi.iItem   = i;
				lvi.pszText = szBuffer;
				lvi.lParam  = (LONG)(*Iter);
				m_ListData.InsertItem(&lvi);


				CSlice *  pSlice = pDoc->GetSlice(*Iter);

				int nSubItemPos = 1;
				std::vector<int>::iterator  SubItemIter = m_vctGroupItem.begin();
				std::vector<int>::iterator  EndOfSubItemIter = m_vctGroupItem.end();
				while(SubItemIter != EndOfSubItemIter)
				{
					pSlice->GetDescriptorValue(*SubItemIter, 1, szBuffer, NULL);
				
					m_ListData.SetItemText(i, nSubItemPos, szBuffer);

					++nSubItemPos;
					++SubItemIter;
				}

			if(posCurSelected == *Iter)
				m_ListData.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
		
			++Iter;
		  
		}
	}
   
    m_ListData.SetRedraw(TRUE);
}


void CFruitDataView::OnCurMeasurementChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW    pNM = (LPNMLISTVIEW)pNMHDR;
	int iSelMeasurement = pNM->iSubItem;
	if(0==iSelMeasurement)
		return;

	--iSelMeasurement;

	int nMeasurementNumber = m_vctGroupItem.size();
	if(nMeasurementNumber<=iSelMeasurement)
		return;

	int nCurMeasurementIndex = m_vctGroupItem[iSelMeasurement];

	CTomatoAnalyzerDoc *    pDoc = GetDocument();
	pDoc->SetSelectedDrawIndex(nCurMeasurementIndex);
}

void CFruitDataView::OnCurFruitChanged()
{
	CTomatoAnalyzerDoc *    pDoc = GetDocument();
	POSITION    posCurFruit = pDoc->GetSelectedObject();

	LVITEM  lvi;
	memset(&lvi, 0, sizeof(LVITEM));
	lvi.mask = LVIF_PARAM;

	int nFruitNumber = m_ListData.GetItemCount();
	for(int i=0; i<nFruitNumber; i++)
	{
		lvi.iItem = i;
		m_ListData.GetItem(&lvi);

		if(lvi.lParam == (LPARAM) posCurFruit)
		{
			m_ListData.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
			m_ListData.EnsureVisible(i, FALSE);
			break;
		}
	}
}

void CFruitDataView::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNM = (LPNMLISTVIEW)pNMHDR;
	
	if(LVIF_STATE == (pNM->uChanged & LVIF_STATE))
	{
		if(LVIS_SELECTED == (LVIS_SELECTED & pNM->uNewState))
		{
			CTomatoAnalyzerDoc * pDoc = (CTomatoAnalyzerDoc *)GetDocument();

			LVITEM  lvi;
			memset(&lvi, 0, sizeof(LVITEM));
			lvi.mask  = LVIF_PARAM;
			lvi.iItem = pNM->iItem;
			m_ListData.GetItem(&lvi);

			pDoc->SetSelectedObject((POSITION) lvi.lParam);
			pDoc->UpdateAllViews(this, DOC_UPDATEVIEW_FRUITSWITCH);
		}
	}

	*pResult = 0;
}
