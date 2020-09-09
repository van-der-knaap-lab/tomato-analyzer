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
// SliceInfoFormView.cpp : implementation file
//

#include "stdafx.h"
#include "BoundingBoxList.h"
#include "Slice.h"
#include "TomatoAnalyzerDoc.h"
#include "tomatoanalyzer.h"
#include "SliceInfoFormView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSliceInfoFormView

IMPLEMENT_DYNCREATE(CSliceInfoFormView, CFormView)

char * szAttribute[]={"Perimeter", "Area", "Fruit Shape Index", "Elliptic", \
                      "Circular", "Rectangle", "Blk Blossom_end", "Blk Stem_end", \
                      "Cylindral", "Obovate", "Ovate", \
                      "Bumpy", "Pointy", "PearShape", "HeartShape", "Taperness", \
                      "Height", "Width", "Shoulder Height", "Indentation", "Large Region Angle", \
                      "Triangle", "Superquadrics n"};


char * getMeasurementName(int i, int& nMethod)
{
	switch(i)
	{
	case 0: // Area
		 nMethod = 1;  
		 break;

	case 1: // Perimeter
		 nMethod = 1;
		 break;

	case 2:   //Aspect Ratio
		 nMethod = 2;
		 break;

	case 3: // Elliptic
		 nMethod = 3;
		 break;

    case 4: // Circular
         nMethod = 3;
         break;

    case 5: // Rectangle
         nMethod = 2;
         break;

    case 6: // Block Blossom End
         nMethod = 1;
         break;

    case 7: //Block Stem end
         nMethod = 1;
         break;

    case 8: //Cylindral
         nMethod = 3;
         break;

    case 9: //Obovate
         nMethod = 1;
         break;

    case 10: //Ovate
         nMethod = 1;
         break;

    case 11: //Bumpy
         nMethod = 2;
         break;

    case 12: //Pointy
         nMethod = 2;
         break;

    case 13: //
         nMethod = 0;
         break;

    case 14: //
         nMethod = 0;
         break;

    case 15:
         nMethod = 1;
         break;

    case 16: //height
         nMethod = 1;
         break;

    case 17: //width
         nMethod = 1;
         break;

    case 18:
         nMethod = 2;
         break;

    case 19:
         nMethod = 1;
         break;

    case 20:
         nMethod = 2;
         break;

    case 21:
         nMethod = 1;
         break;

    case 22:
         nMethod = 1;
         break;




    default:
         nMethod = 0;
         break;

    }

    return szAttribute[i];
}


int getMeasurementNumber()
{
    return sizeof(szAttribute)/sizeof(char *);
}




CSliceInfoFormView::CSliceInfoFormView()
	: CFormView(CSliceInfoFormView::IDD)
{
	//{{AFX_DATA_INIT(CSliceInfoFormView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CSliceInfoFormView::~CSliceInfoFormView()
{
}

void CSliceInfoFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSliceInfoFormView)
	DDX_Control(pDX, IDC_SLICEINFO_LIST, m_data);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSliceInfoFormView, CFormView)
	//{{AFX_MSG_MAP(CSliceInfoFormView)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SLICEINFO_LIST, OnItemchangedSliceInfoList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSliceInfoFormView diagnostics

#ifdef _DEBUG
void CSliceInfoFormView::AssertValid() const
{
	CFormView::AssertValid();
}

void CSliceInfoFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSliceInfoFormView message handlers

void CSliceInfoFormView::OnInitialUpdate() 
{
	RECT rt;
	GetClientRect(&rt);

	TCHAR *	ppszColName[]={"Attribute", "Method I", "Method II", "Method III"};
	CFormView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	SIZE	sz={0,0};
	SetScrollSizes(MM_TEXT, sz);

	m_data.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_data.SetItemCount(4);

    LVCOLUMN	lvc; 
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
    lvc.fmt = LVCFMT_LEFT; 
    lvc.cx = 115; 
	lvc.iSubItem = 0;

	int nWidth = (rt.right - rt.left)/(sizeof(ppszColName)/sizeof(CHAR *));
	for(int iloop=0; iloop<sizeof(ppszColName)/sizeof(CHAR *); iloop++)
	{
	    lvc.pszText = ppszColName[iloop];
		m_data.InsertColumn(iloop, &lvc);
	}


 	LVITEM	lvi;
	
	lvi.mask = LVIF_TEXT|LVIF_STATE|LVIF_PARAM; 
    lvi.state = 0; 
    lvi.stateMask = 0; 
    lvi.iImage = 0;
    lvi.iSubItem = 0; 
    lvi.lParam = NULL;    
	for(int iloop=0; iloop<sizeof(szAttribute)/sizeof(TCHAR *); iloop++)
	{
		lvi.pszText=szAttribute[iloop];
	    lvi.iItem = iloop;
		m_data.InsertItem(&lvi);
	}

	return;
}

void CSliceInfoFormView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// TODO: Add your specialized code here and/or call the base class
	CTomatoAnalyzerDoc * pDoc=(CTomatoAnalyzerDoc *)GetDocument();
	if(pDoc->m_bAnalyzed == FALSE || pDoc->GetSelectedObject () == NULL)
	{
		for(int i=0; i<sizeof(szAttribute)/sizeof(TCHAR *); i++)
        {
            for(int iCol=1; iCol<=3; iCol++)
                m_data.SetItemText(i, iCol, "");
        }

		return;
	}
	
	TCHAR	szBuffer[256];
	CSlice * pResult=pDoc->GetAnalyzeResult(NULL);
	for( int i=0; i<sizeof(szAttribute)/sizeof(char *); i++)
	{

		bool bNext;
		int  iMethod=0;

		do{
			memset(szBuffer,0,256);
			bNext = pResult->GetDescriptorValue(i, iMethod, szBuffer, NULL);
			m_data.SetItemText(i, iMethod+1, szBuffer);

			iMethod++;
		}while(bNext);
	}
 }

void CSliceInfoFormView::OnItemchangedSliceInfoList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	if(LVIF_STATE == (pNMListView->uChanged & LVIF_STATE))
	{
		if(LVIS_SELECTED == (LVIS_SELECTED & pNMListView->uNewState))
		{
			CTomatoAnalyzerDoc * pDoc = (CTomatoAnalyzerDoc *)GetDocument();
			pDoc->SetSelectedDrawIndex(pNMListView->iItem);
		}
	}
}

void CSliceInfoFormView::CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType) 
{
	// TODO: Add your specialized code here and/or call the base class
	CFormView::CalcWindowRect(lpClientRect, nAdjustType);
}
