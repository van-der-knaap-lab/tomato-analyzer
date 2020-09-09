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
#include "stdafx.h"
#include "tomatoanalyzer.h"
#include "EllipseTracker.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CEllipseTracker::CEllipseTracker()
{
	m_nCurTrackedPoint = -1;
    m_blOnlyTipMovable = false;
}

CEllipseTracker::~CEllipseTracker()
{
}

	
BOOL CEllipseTracker::Track(CView * pView, CBitmap * pBitmap, POINT * pt)
{
	// get the drawing tool??? or something
	CDC* pDrawDC = pView->GetDC();
	ASSERT_VALID(pDrawDC);

	CDC* pConvertDC = pView->GetDC();
	ASSERT_VALID(pConvertDC);
    pView->OnPrepareDC(pConvertDC);

	// ?
    POINT   pt1, ptt;
    pt1.x = pt1.y = 0;
    ptt.x = ptt.y = 3;
    pConvertDC->DPtoLP(&pt1, 1);
    pConvertDC->DPtoLP(&ptt, 1);
    m_rectX = ptt.x - pt1.x;
    m_rectY = ptt.y - pt1.y;

		
	for(int i=0; i<3; i++)
	{
		ellipse_points[i] = pt[i];
	}



    pView->SetCapture();

    RECT    rect;
    pView->GetClientRect(&rect);
    m_nViewWidth = rect.right + 1;
    m_nViewHeight = rect.bottom + 1;

	BITMAP	bt;
	pBitmap->GetBitmap(&bt);
    m_ptLeftTop.x = ((m_nViewWidth-bt.bmWidth)>>1);
    m_ptLeftTop.y = ((m_nViewHeight-bt.bmHeight)>>1);

    m_ptDiffIndi.x = m_ptDiffIndi.y = 0;
   

    Draw(pView, pDrawDC, pBitmap);

	BOOL bContinue = TRUE;
	BOOL bConfirm  = FALSE;
	//**MODIFICATION** pulled in nClickedPt; out of the while loop.
	int nClickedPt;
	while(bContinue)
	{
		MSG msg;
		VERIFY(::GetMessage(&msg, NULL, 0, 0));
	
		
        
		switch (msg.message)
		{
		case WM_LBUTTONDOWN:
            pt1.x = (int)LOWORD(msg.lParam);  
            pt1.y = (int)HIWORD(msg.lParam);
            pConvertDC->DPtoLP(&pt1, 1);
            nClickedPt = ClickOnIndicator(pt1);
            if(nClickedPt != -1)
            {
                m_nCurTrackedPoint = nClickedPt;
				m_ptDiffIndi.x = ellipse_points[nClickedPt].x - pt1.x;
				m_ptDiffIndi.y = ellipse_points[nClickedPt].y - pt1.y;
			    Draw(pView, pDrawDC, pBitmap);
            }
			break;

        case WM_LBUTTONUP:
            if(m_nCurTrackedPoint != -1)
            {
                m_nCurTrackedPoint = -1;
                m_ptDiffIndi.x = m_ptDiffIndi.y =0;

                Draw(pView, pDrawDC, pBitmap);
            }
            break;

        case WM_MOUSEMOVE:
            if(m_nCurTrackedPoint != -1)
            {
                pt1.x = (int)LOWORD(msg.lParam);  
                pt1.y = (int)HIWORD(msg.lParam);
                pConvertDC->DPtoLP(&pt1, 1);
				ellipse_points[nClickedPt].x = pt1.x + m_ptDiffIndi.x;
				ellipse_points[nClickedPt].y = pt1.y + m_ptDiffIndi.y;
                
                Draw(pView, pDrawDC, pBitmap);
            }
            break;

		case WM_KEYDOWN:
			if(msg.wParam == VK_ESCAPE)
            {
				bContinue = FALSE;
                pView->InvalidateRect(NULL, TRUE);
            }
			else if(msg.wParam == VK_RETURN)
			{
				bContinue = FALSE;
                bConfirm = TRUE;
			}
			break;

        case WM_LBUTTONDBLCLK:
			bContinue = FALSE;
            bConfirm = TRUE;
            break;

		// just dispatch rest of the messages
		default:
			DispatchMessage(&msg);
			break;
		}
	}

    ReleaseCapture();
	pView->ReleaseDC(pDrawDC);
    pView->ReleaseDC(pConvertDC);

    return bConfirm;

}

int CEllipseTracker::ClickOnIndicator(POINT pt)
{
    int nFrom, nTo;

    if(m_blOnlyTipMovable)
        nFrom = nTo = 1;
    else
    {
        nFrom = 0;
        nTo = 2;
    }
	
    for(int i=nFrom; i<=nTo; i++)
    {
        int x = abs(pt.x - ellipse_points[i].x);
        int y = abs(pt.y - ellipse_points[i].y);
        BOOL bRet = (x<=3 && y<=3)? TRUE: FALSE;

        if(bRet)
            return i;
    }

    return -1;
}


void CEllipseTracker::Draw(CView* pView, CDC * pOrgDC, CBitmap * pBitmap)
{
    RECT    rect;
    CBitmap OrgBmp;
    CBitmap* oldBMOrg, * oldBM;

    CDC* pDC = new CDC;
    pDC->CreateCompatibleDC(pOrgDC);
    OrgBmp.CreateCompatibleBitmap(pOrgDC, m_nViewWidth, m_nViewHeight);
    oldBMOrg = pDC->SelectObject(&OrgBmp);

    rect.left  = 0;
    rect.right = m_nViewWidth-1;
    rect.top   = 0;
    rect.bottom= m_nViewHeight-1;
    pDC->SelectObject(GetStockObject(BLACK_PEN));
    pDC->SelectObject(GetStockObject(BLACK_BRUSH));
    pDC->Rectangle(&rect);

	BITMAP	bt;
	pBitmap->GetBitmap(&bt);

	CDC * MemDC = new CDC;
	MemDC->CreateCompatibleDC(pDC);
	oldBM = MemDC->SelectObject(pBitmap);

    int nSaveIndexMem = pDC->SaveDC();
    pView->OnPrepareDC(pDC);
    pDC->SetMapMode(MM_TEXT);
    pDC->BitBlt(0, 0, bt.bmWidth, bt.bmHeight, MemDC, 0, 0, SRCCOPY);
	MemDC->SelectObject(oldBM);
	delete MemDC;

    pView->OnPrepareDC(pDC);

    int nFrom, nTo;
    if(m_blOnlyTipMovable)
        nFrom = nTo = 1;
    else
    {
        nFrom = 0;
        nTo = 2;
    }

    pDC->SelectObject(GetStockObject(NULL_PEN));
    for(int i=nFrom; i<=nTo; i++)
    {
        rect.left  = ellipse_points[i].x - m_rectX;
        rect.right = ellipse_points[i].x + m_rectX;
        rect.top   = ellipse_points[i].y - m_rectY;
        rect.bottom= ellipse_points[i].y + m_rectY;
    
        HBRUSH  hBrush, hBrushOld;
        hBrush = CreateSolidBrush((i==m_nCurTrackedPoint)? RGB(0,255,0): RGB(0,200,0));
        hBrushOld = (HBRUSH) pDC->SelectObject(hBrush);
        pDC->Rectangle(&rect);
        pDC->SelectObject(hBrushOld);
        DeleteObject(hBrush);
    }

    if(false == m_blOnlyTipMovable)
    {
        pDC->SelectObject(GetStockObject(WHITE_PEN));
        pDC->MoveTo(ellipse_points[0]);
        pDC->LineTo(ellipse_points[1]);
        pDC->LineTo(ellipse_points[2]);
    }

    pDC->SetViewportOrg(0,0);

    pDC->RestoreDC(nSaveIndexMem);
	pOrgDC->BitBlt(0, 0, m_nViewWidth, m_nViewHeight, pDC, 0, 0, SRCCOPY);
    pDC->SelectObject(oldBMOrg);
    delete pDC;

    OrgBmp.DeleteObject();
    return;
}


void CEllipseTracker::GetResult(POINT * ppt)
{
    ppt[0] = ellipse_points[0];
    ppt[1] = ellipse_points[1];
    ppt[2] = ellipse_points[2];

    return;
}

