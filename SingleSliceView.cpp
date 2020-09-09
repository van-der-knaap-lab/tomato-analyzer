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
// SingleSliceView.cpp : implementation file
//

#include "stdafx.h"
#include "BoundingBoxList.h"
#include "slice.h"
#include "tomatoanalyzerdoc.h"
#include "tomatoanalyzer.h"
#include "SingleSliceView.h"
#include "new_img_function.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define pi 3.1415926

/////////////////////////////////////////////////////////////////////////////
// CSingleSliceView

IMPLEMENT_DYNCREATE(CSingleSliceView, CView)

CSingleSliceView::CSingleSliceView()
{
	m_bFirstClickForRotate = FALSE;

	m_pBitmap = NULL;

    m_fScale = 1.0;
    m_imgw = m_imgh = 0;
}

CSingleSliceView::~CSingleSliceView()
{
	if(m_pBitmap != NULL)
	{
		delete m_pBitmap;
	}
}


BEGIN_MESSAGE_MAP(CSingleSliceView, CView)
	//{{AFX_MSG_MAP(CSingleSliceView)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSingleSliceView drawing

void CSingleSliceView::OnDraw(CDC* pDC)
{
	//first, display the single slice image
	CTomatoAnalyzerDoc* pDoc = (CTomatoAnalyzerDoc*)GetDocument();

	POSITION pos = pDoc->GetSelectedObject();
	if(NULL == pos)
		return;

	CSlice* pSlice = pDoc->m_SliceList.GetAt(pos);

	DrawImage(pDC);

	int iDrawIndedx = pDoc->GetSelectedDrawIndex();
    pSlice->DrawVisualEffect(pDC, iDrawIndedx);

    return;
}

/////////////////////////////////////////////////////////////////////////////
// CSingleSliceView diagnostics

#ifdef _DEBUG
void CSingleSliceView::AssertValid() const
{
	CView::AssertValid();
}

void CSingleSliceView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSingleSliceView message handlers

BOOL CSingleSliceView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	CRect rect;
	pDC->GetClipBox(&rect);

	CBrush brush(RGB(0,0,0));
	CBrush *pOldBrush = pDC->SelectObject(&brush);

	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);

	pDC->SelectObject(pOldBrush);

	return TRUE;
}

void CSingleSliceView::DrawImage(CDC *pDC)
{
    if(m_pBitmap == NULL)
        return;

    int dcIndex = pDC->SaveDC();
    pDC->SetMapMode(MM_TEXT);

    BITMAP	bt;
	m_pBitmap->GetBitmap(&bt);

	CDC * MemDC = new CDC;
	MemDC->CreateCompatibleDC(pDC);
	CBitmap* oldBM = MemDC->SelectObject(m_pBitmap);
		
	pDC->BitBlt(0, 0, bt.bmWidth, bt.bmHeight, MemDC, 0, 0, SRCCOPY);
	delete MemDC;

    pDC->RestoreDC(dcIndex);
}


void CSingleSliceView::OnMouseMove(UINT nFlags, CPoint point)
{
	if(!m_bFirstClickForRotate)
		return;


	CClientDC	dc(this);

	CPen * pPen = new CPen(PS_SOLID, 2, RGB(255,0,0));
	CPen * pOldPen = dc.SelectObject(pPen);
	
	int nOldROP2 = dc.SetROP2(R2_XORPEN);

	dc.MoveTo(m_ptRotate[0]);
	dc.LineTo(m_ptRotate[1]);

	m_ptRotate[1]=point;
	dc.MoveTo(m_ptRotate[0]);
	dc.LineTo(m_ptRotate[1]);

	dc.SetROP2(nOldROP2);
	dc.SelectObject(pOldPen);
	delete pPen;
	
	return;
}

void CSingleSliceView::OnRButtonDown(UINT nFlags, CPoint point) 
{
}

void CSingleSliceView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	return;
}


BOOL CSingleSliceView::RotateSliceOperation(BOOL bRotate)
{
	if(!bRotate)
	{
		if(m_bFirstClickForRotate)
			ReleaseCapture();
		
		m_bFirstClickForRotate = FALSE;
	}

	return bRotate;
}

void CSingleSliceView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	CView::OnPrepareDC(pDC, pInfo);
	
    POINT Org = GetOffsetPoint();
	pDC->SetViewportOrg(Org);

    if(m_fScale != 1.0)
    {
        pDC->SetMapMode(MM_ANISOTROPIC);

        pDC->SetWindowOrg(0, 0);

        pDC->SetWindowExt(m_imgw, m_imgh);
        pDC->SetViewportExt(m_fScale*double(m_imgw), m_fScale*double(m_imgh));
    }
}

void CSingleSliceView::CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType) 
{
	// TODO: Add your specialized code here and/or call the base class
	CView::CalcWindowRect(lpClientRect, nAdjustType);

}

POINT CSingleSliceView::GetOffsetPoint()
{
	POINT Org;
	Org.x = Org.y = 0;

	CRect winRect;
	GetWindowRect(&winRect);
	Org.x = (winRect.Width () - m_fScale * double(m_imgw) )/2;
	Org.y = (winRect.Height() - m_fScale * double(m_imgh) )/2;
    return Org;
}

void CSingleSliceView::OnMButtonDown(UINT nFlags, CPoint point)
{
}

void CSingleSliceView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
    CView::OnUpdate(pSender, lHint, pHint);
    if(DOC_UPDATEVIEW_MEASURECHANGED==lHint || DOC_UPDATEVIEW_FRUITESTATE==lHint)
    {
        InvalidateRect(NULL, TRUE);
        return;
    }


	// TODO: Add your specialized code here and/or call the base class
	CTomatoAnalyzerDoc* pDoc = (CTomatoAnalyzerDoc*)GetDocument();

	POSITION pos = pDoc->GetSelectedObject();
	if(NULL == pos)
		return;

    CSlice * pSlice = pDoc->GetSlice(pos);
    RECT  rect = pSlice->mBBox.GetRect();
	CAdvImage *pSliceImage = pDoc->m_pImage->Crop(rect);
    if(NULL == pSliceImage)
        return;
    
    double angle = pSlice->GetRotationDegree();
    if(0.0 != angle)
    {
        CAdvImage * pTmp = pSliceImage->Rotate(angle);
        delete pSliceImage;
        pSliceImage = pTmp;
    }

    m_imgw = pSliceImage->GetWidth();
    m_imgh = pSliceImage->GetHeight();
    
    RECT    viewRect;
    GetClientRect(&viewRect);
    int vieww = viewRect.right - viewRect.left + 1;
    int viewh = viewRect.bottom - viewRect.top + 1;

    if(m_imgw*5>vieww*4 || m_imgh*5>viewh*4)
    {
        double tmpw = double(vieww*4)/double(m_imgw*5);
        double tmph = double(viewh*4)/double(m_imgh*5);
        m_fScale = (tmpw<tmph)? tmpw : tmph;
    }
    else
        m_fScale = 1.0;

    if(m_pBitmap != 0)
    {
        delete m_pBitmap;
        m_pBitmap = 0;
    }

    CDC dc;
    dc.CreateDC("DISPLAY", NULL, NULL, NULL);
    m_pBitmap = CreateBitmap(&dc, pSliceImage, m_fScale, "near");

    delete pSliceImage;
	return;
}

void CSingleSliceView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	OnUpdate(0, 0, 0);
}
