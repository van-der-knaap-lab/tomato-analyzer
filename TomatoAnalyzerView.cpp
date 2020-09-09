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
// TomatoAnalyzerView.cpp : implementation of the CTomatoAnalyzerView class
//

#include "stdafx.h"
#include "resource.h"
#include "BoundingBoxList.h"
#include "AdvImage.h"
#include "Slice.h"
#include "TomatoAnalyzer.h"
#include "TomatoAnalyzerDoc.h"
#include "TomatoAnalyzerView.h"

#include "new_img_Function.h"

#include "math.h"
#include "Matrix.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTomatoAnalyzerView

IMPLEMENT_DYNCREATE(CTomatoAnalyzerView, CScrollView)

BEGIN_MESSAGE_MAP(CTomatoAnalyzerView, CScrollView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)

	ON_COMMAND_RANGE(ID_ZOOM, ID_ZOOM200, OnCmdZoomView)
	
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_ERASEBKGND()

END_MESSAGE_MAP()

// CTomatoAnalyzerView construction/destruction

CTomatoAnalyzerView::CTomatoAnalyzerView()
{
	// TODO: add construction code here
	m_fZoom = 0.0;
	m_pBitmap = NULL;

	m_ptExclude[0].x = m_ptExclude[0].y = -1;
	m_ptExclude[1].x = m_ptExclude[1].y = -1;
}

CTomatoAnalyzerView::~CTomatoAnalyzerView()
{
	if( NULL != m_pBitmap)
		delete m_pBitmap;
}

BOOL CTomatoAnalyzerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CTomatoAnalyzerView drawing
void CTomatoAnalyzerView::OnDraw(CDC* pDC)
{
	CTomatoAnalyzerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
	pDC->SetBkColor(RGB(0, 0, 0));

	if(NULL != m_pBitmap)
	{
		BITMAP	bt;
		m_pBitmap->GetBitmap(&bt);

		CDC * MemDC = new CDC;
		MemDC->CreateCompatibleDC(pDC);
		CBitmap* oldBM = MemDC->SelectObject(m_pBitmap);
		
		pDC->BitBlt(0, 0, bt.bmWidth, bt.bmHeight, MemDC, 0, 0, SRCCOPY);
		MemDC->SelectObject(oldBM);
		delete MemDC;

        CPoint  ptViewOrg = pDC->GetViewportOrg();
        pDC->SetMapMode(MM_ANISOTROPIC);
        pDC->SetWindowOrg(0, 0);

    	int iDrawIndex = pDoc->GetSelectedDrawIndex();
        CSlice* pCurSelectedSlice = pDoc->GetSelectedSlice();
        if(NULL == pCurSelectedSlice)
            iDrawIndex = INDEX_PERIMETER;

		POSITION pos = pDoc->m_SliceList.GetHeadPosition();
		while(pos)
		{
			CSlice* pSlice = pDoc->m_SliceList.GetNext(pos);

            RECT    rect;
            pSlice->GetDisplayRegion(&rect);

            int nImgW = rect.right - rect.left + 1;
            int nImgH = rect.bottom - rect.top + 1;
            pDC->SetWindowExt(nImgW, nImgH);
            pDC->SetViewportExt(m_fZoom*double(nImgW)+0.5,  m_fZoom*double(nImgH)+0.5);

            int viewOrgX = ptViewOrg.x + round(m_fZoom * double(rect.left));
            int viewOrgY = ptViewOrg.y + round(m_fZoom * double(rect.top));
	        pDC->SetViewportOrg(viewOrgX, viewOrgY);

            if(! pSlice->GetExclusionFlag())
                pSlice->DrawVisualEffect(pDC, iDrawIndex);
            else
                pSlice->DrawVisualEffect(pDC, INDEX_PERIMETER);

            if(pSlice == pCurSelectedSlice)
                pSlice->DrawBBox(pDC, 1.0);
		}
	}
}


// CTomatoAnalyzerView printing

BOOL CTomatoAnalyzerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CTomatoAnalyzerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CTomatoAnalyzerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

// CTomatoAnalyzerView diagnostics

#ifdef _DEBUG
void CTomatoAnalyzerView::AssertValid() const
{
	CView::AssertValid();
}

void CTomatoAnalyzerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTomatoAnalyzerDoc* CTomatoAnalyzerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTomatoAnalyzerDoc)));
	return (CTomatoAnalyzerDoc*)m_pDocument;
}
#endif //_DEBUG


void CTomatoAnalyzerView::OnCmdZoomView(UINT nID)
{
	double fscale[]={0.0f, 0.2f, 0.5f, 0.8f, 1.0f, 1.50f, 2.0f};
	
	int iIndex = nID - ID_ZOOM;
	if(iIndex>=0 && iIndex<=sizeof(fscale)/sizeof(double))
	{
		if(m_fZoom != fscale[iIndex])
		{
			m_fZoom = fscale[iIndex];
			OnUpdate(NULL, 0, NULL);
		}
	}
	return;
}

void CTomatoAnalyzerView::OnInitialUpdate()
{
	m_fZoom = 0.0;

	CScrollView::OnInitialUpdate();
}

void CTomatoAnalyzerView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    if(DOC_UPDATEVIEW_FRUITSWITCH == lHint || DOC_UPDATEVIEW_MEASURECHANGED==lHint)
    {
        InvalidateRect(NULL, TRUE);
        return;
    }


	SIZE	sz;

	if(NULL != m_pBitmap)
	{
		delete m_pBitmap;
		m_pBitmap = NULL;
	}

	CTomatoAnalyzerDoc *  pDoc = (CTomatoAnalyzerDoc *)GetDocument();
	CAdvImage * pImage = pDoc->GetImage();
	
	if(NULL == pImage)
	{
		sz.cx = sz.cy = 0;
	}
	else
	{
		int nImgWidth  = pImage->GetWidth();
		int nImgHeight = pImage->GetHeight();

		if(m_fZoom <= 0.0)
		{
			RECT  rt;
			GetClientRect(&rt);

			if(rt.right>=nImgWidth && rt.bottom>=nImgHeight)
				m_fZoom=1.0;
			else
			{
				double fx=double(rt.right)/double(nImgWidth);
				double fy=double(rt.bottom)/double(nImgHeight);

				m_fZoom = (fx<fy)? fx : fy;
			}
		}
		
		CClientDC dc(NULL);
        if( pDoc->IsThereAnySliceRotated() )
        {
            CAdvImage * pFullImage = pImage->GetCopy();

            POSITION pos = pDoc->m_SliceList.GetHeadPosition();
		    while(pos)
		    {
			    CSlice* pSlice = pDoc->m_SliceList.GetNext(pos);
                double angle = pSlice->GetRotationDegree();
                if(angle != 0.0)
                    this->RenewImageAfterSliceRotation(pFullImage, pSlice);
            }

            m_pBitmap = CreateBitmap(&dc, pFullImage, m_fZoom, "near");
            delete pFullImage;
        }
        else
        {
            m_pBitmap = CreateBitmap(&dc, pImage, m_fZoom, "near");
        }

		sz.cx = (int) floor(double(nImgWidth) * m_fZoom);
		sz.cy = (int) floor(double(nImgHeight) * m_fZoom);
	}


	SetScrollSizes(MM_TEXT, sz);

	CScrollView::OnUpdate(pSender, lHint, pHint);
}


BOOL CTomatoAnalyzerView::OnEraseBkgnd(CDC* pDC) 
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

void CTomatoAnalyzerView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CTomatoAnalyzerDoc * pDoc = GetDocument();

	CClientDC	dc(this);
	OnPrepareDC(&dc);	
	dc.DPtoLP(&point);

	point.x = (int) floor(double(point.x) / m_fZoom);
	point.y = (int) floor(double(point.y) / m_fZoom);

	POSITION pos = pDoc->DecideClickedObject(point);
	if(NULL != pos)
	{
		CSlice * pSlice = pDoc->GetSlice(pos);
		pSlice->InvertExclusionFlag();
		InvalidateRect(NULL, TRUE);

        pDoc->UpdateAllViews(NULL, DOC_UPDATEVIEW_FRUITESTATE);
	}

	return;
}


void CTomatoAnalyzerView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default.
	CTomatoAnalyzerDoc* pDoc = GetDocument();
	if(!pDoc->IsAnalyzed())
	return;

	SetCapture();

	CClientDC	dc(this);
	OnPrepareDC(&dc);	
	dc.DPtoLP(&point);
	m_ptExclude[0] = m_ptExclude[1] = point;

	return;
}

void CTomatoAnalyzerView::OnMouseMove(UINT nFlags, CPoint point)
{
	if( GetCapture()!=this || (MK_LBUTTON & nFlags)!=MK_LBUTTON)
		return;

	CClientDC	dc(this);
	OnPrepareDC(&dc);	
	dc.DPtoLP(&point);

	CPen * pPen = new CPen(PS_DOT, 1, RGB(0,0,255));
	CPen * pOldPen = dc.SelectObject(pPen);
	dc.SelectObject(GetStockObject(NULL_BRUSH));
	
	int nOldROP2 = dc.SetROP2(R2_XORPEN);
	dc.Rectangle(m_ptExclude[0].x, m_ptExclude[0].y, m_ptExclude[1].x, m_ptExclude[1].y);

	m_ptExclude[1]=point;
	dc.Rectangle(m_ptExclude[0].x, m_ptExclude[0].y, m_ptExclude[1].x, m_ptExclude[1].y);

	dc.SetROP2(nOldROP2);
	dc.SelectObject(pOldPen);
	delete pPen;

	return;
}


void CTomatoAnalyzerView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(GetCapture() == this)
		ReleaseCapture();

	if(m_ptExclude[0] != m_ptExclude[1])
	{
		CClientDC	dc(this);
		OnPrepareDC(&dc);	

		CPen * pPen = new CPen(PS_DOT, 1, RGB(0,0,255));
		CPen * pOldPen = dc.SelectObject(pPen);
		dc.SelectObject(GetStockObject(NULL_BRUSH));
	
		dc.SetROP2(R2_XORPEN);
		dc.Rectangle(m_ptExclude[0].x, m_ptExclude[0].y, m_ptExclude[1].x, m_ptExclude[1].y);

		dc.SetROP2(R2_COPYPEN);
		dc.SelectObject(pOldPen);
		delete pPen;

		int tmp;
	if(m_ptExclude[0].x > m_ptExclude[1].x)
		{
			tmp = m_ptExclude[1].x;
			m_ptExclude[1].x = m_ptExclude[0].x;
			m_ptExclude[0].x = tmp;
		}

		if(m_ptExclude[0].y > m_ptExclude[1].y)
		{
			tmp = m_ptExclude[1].y;
			m_ptExclude[1].y = m_ptExclude[0].y;
			m_ptExclude[0].y = tmp;
		}

		m_ptExclude[0].x = (int) floor( double(m_ptExclude[0].x)/m_fZoom );
		m_ptExclude[0].y = (int) floor( double(m_ptExclude[0].y)/m_fZoom );
		m_ptExclude[1].x = (int) floor( double(m_ptExclude[1].x)/m_fZoom );
		m_ptExclude[1].y = (int) floor( double(m_ptExclude[1].y)/m_fZoom );

		CTomatoAnalyzerDoc* pDoc = GetDocument();
		POSITION pos = pDoc->m_SliceList.GetHeadPosition();
		while(pos)
		{
			CSlice* pSlice = pDoc->m_SliceList.GetNext(pos);

			if( m_ptExclude[0].x < pSlice->mBBox.mUpperLeft.x &&
				m_ptExclude[0].y < pSlice->mBBox.mUpperLeft.y &&
				m_ptExclude[1].x > pSlice->mBBox.mLowerRight.x &&
				m_ptExclude[1].y > pSlice->mBBox.mLowerRight.y)
			{
				pSlice->InvertExclusionFlag();
				InvalidateRect(NULL, TRUE);
			}
		}

		m_ptExclude[0].x = m_ptExclude[0].y = -1;
		m_ptExclude[1].x = m_ptExclude[1].y = -1;

        pDoc->UpdateAllViews(NULL, DOC_UPDATEVIEW_FRUITESTATE);

	}
	else
	{
		SelectSlice(point);
	}

	return;
}


VOID CTomatoAnalyzerView::SelectSlice(CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CTomatoAnalyzerDoc * pDoc = GetDocument();

	CClientDC	dc(this);
	OnPrepareDC(&dc);	
	dc.DPtoLP(&point);

	point.x = (int) floor(double(point.x) / m_fZoom);
	point.y = (int) floor(double(point.y) / m_fZoom);

	POSITION old = pDoc->GetSelectedObject();
	POSITION pos = pDoc->DecideClickedObject(point);

	if(NULL != pos && pos!=old)
	{
		pDoc->SetSelectedObject(pos);
        InvalidateRect(NULL, TRUE);
		pDoc->UpdateAllViews(this, DOC_UPDATEVIEW_FRUITSWITCH);
	}
}


void CTomatoAnalyzerView::RenewImageAfterSliceRotation(CDC * pDC, CBitmap * pBtmp, CSlice * pSlice)
{
	CTomatoAnalyzerDoc * pDoc = GetDocument();
  
    RECT orgRect = pSlice->mBBox.GetRect();
	CAdvImage *pSliceImage = pDoc->m_pImage->Crop(orgRect);
    if(NULL == pSliceImage)
        return;
    
    double angle = pSlice->GetRotationDegree();
    if(0.0 != angle)
    {
        CAdvImage * pTmp = pSliceImage->Rotate(angle);
        delete pSliceImage;
        pSliceImage = pTmp;
    }

    CBitmap * pRotbmp = CreateBitmap(pDC, pSliceImage);


    CDC dcDst;
    dcDst.CreateCompatibleDC(pDC);
    CBitmap * pOldBitmapDst = dcDst.SelectObject(pBtmp);

    CDC dcSrc;
    dcSrc.CreateCompatibleDC(pDC);
    CBitmap * pOldBitmapSrc = dcSrc.SelectObject(pRotbmp);

    int imgW = pSliceImage->GetWidth();
    int imgH = pSliceImage->GetHeight();
    int nX = orgRect.left + ((orgRect.right - orgRect.left + 1 - imgW)>>1);
    int nY = orgRect.top + ((orgRect.bottom - orgRect.top + 1 - imgH)>>1);
	dcDst.BitBlt(nX, nY, imgW, imgW, &dcSrc, 0, 0, SRCCOPY);
    
    dcDst.SelectObject(pOldBitmapDst);
    dcSrc.SelectObject(pOldBitmapSrc);

    orgRect.left   = nX;
    orgRect.right  = orgRect.left + imgW - 1;
    orgRect.top    = nY;
    orgRect.bottom = orgRect.top + imgH - 1;
    pSlice->SetDisplayRegion(&orgRect);

    delete pRotbmp;
    delete pSliceImage;
    return;
}


void CTomatoAnalyzerView::RenewImageAfterSliceRotation(CAdvImage * pFullImage, CSlice * pSlice)
{
	CTomatoAnalyzerDoc * pDoc = GetDocument();
  
    RECT orgRect = pSlice->mBBox.GetRect();
	CAdvImage *pSliceImage = pFullImage->Crop(orgRect);
    if(NULL == pSliceImage)
        return;
    
    double angle = pSlice->GetRotationDegree();
    if(0.0 != angle)
    {
        CAdvImage * pTmp = pSliceImage->Rotate(angle);
        delete pSliceImage;
        pSliceImage = pTmp;
    }

    int imgW = pSliceImage->GetWidth();
    int imgH = pSliceImage->GetHeight();
    // find the topleft? x,y coords. if they're negative, then make it 0.
    int nX = max(orgRect.left + ((orgRect.right - orgRect.left + 1 - imgW)>>1),0);
    int nY = max(orgRect.top + ((orgRect.bottom - orgRect.top + 1 - imgH)>>1),0);

    pFullImage->Paste(nX, nY, pSliceImage);
    
    orgRect.left   = nX;
    orgRect.right  = orgRect.left + imgW - 1;
    orgRect.top    = nY;
    orgRect.bottom = orgRect.top + imgH - 1;
    pSlice->SetDisplayRegion(&orgRect);

    delete pSliceImage;
    return;
}

