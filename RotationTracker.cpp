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
// RotationTracker.cpp: implementation of the CRotationTracker class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "tomatoanalyzer.h"
#include "AdvImage.h"
#include "RotationTracker.h"
#include "new_img_function.h"

#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CBitmap * CreateBitmap(CDC* pDC, CAdvImage * pImage);


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRotationTracker::CRotationTracker()
{

}

CRotationTracker::~CRotationTracker()
{

}

BOOL CRotationTracker::Track(CView* pView, CAdvImage * pImage, POINT ptOrg, double& angle)
{
	CDC* pDrawDC = pView->GetDC();
	ASSERT_VALID(pDrawDC);

    pView->SetCapture();

    RECT    rect;
    pView->GetClientRect(&rect);
    m_nViewWidth = rect.right + 1;
    m_nViewHeight = rect.bottom + 1;
    m_ptCenter.x = (m_nViewWidth>>1);
    m_ptCenter.y = (m_nViewWidth>>1);
    
    double dTmp = double(m_nViewWidth)/3.0;
    m_ptIndicator.x = m_ptCenter.x - round(dTmp * sin(angle));
    if(m_ptIndicator.x<10)
        m_ptIndicator.x = 10;
    else if(m_ptIndicator.x > m_nViewWidth-10)
        m_ptIndicator.x = m_nViewWidth-10;

    m_ptIndicator.y = m_ptCenter.y - round(dTmp * cos(angle));
    if(m_ptIndicator.y<10)
        m_ptIndicator.y = 10;
    else if(m_ptIndicator.y > m_nViewHeight-10)
        m_ptIndicator.y = m_nViewHeight-10;


    m_ptDiffIndi.x = m_ptDiffIndi.y = 0;

    Draw(pDrawDC, pImage, NULL);

	BOOL bContinue = TRUE;
	BOOL bConfirm  = FALSE;
    BOOL bTracking = FALSE;
	while(bContinue)
	{
		MSG msg;
		VERIFY(::GetMessage(&msg, NULL, 0, 0));

        POINT   pt;
		switch (msg.message)
		{
		case WM_LBUTTONDOWN:
            pt.x = (int)LOWORD(msg.lParam);  
            pt.y = (int)HIWORD(msg.lParam);
            if(ClickOnIndicator(pt))
            {
                bTracking = TRUE;
                m_ptDiffIndi.x = m_ptIndicator.x - pt.x;
                m_ptDiffIndi.y = m_ptIndicator.y - pt.y;
			    Draw(pDrawDC, pImage, &pt);
            }
			break;

        case WM_LBUTTONUP:
            bTracking = FALSE;
            m_ptDiffIndi.x = m_ptDiffIndi.y =0;
		    Draw(pDrawDC, pImage, NULL);
            break;

        case WM_MOUSEMOVE:
            if(TRUE == bTracking)
            {
                pt.x = (int)LOWORD(msg.lParam);  
                pt.y = (int)HIWORD(msg.lParam);
                angle = GetRotationAngle(pt);
                Draw(pDrawDC, pImage, &pt);
            }
            break;

		case WM_KEYDOWN:
			if(msg.wParam == VK_ESCAPE)
            {
				bContinue = FALSE;
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
    
    return bConfirm;
}

BOOL CRotationTracker::ClickOnIndicator(POINT pt)
{
    int x = abs(pt.x - m_ptIndicator.x);
    int y = abs(pt.y - m_ptIndicator.y);
    BOOL bRet = (x<=3 && y<=3)? TRUE: FALSE;

    return bRet;
}

#define pi   3.1415926535897942
double CRotationTracker::GetRotationAngle(POINT ptCur)
{
    double angle = atan2((long double)(m_ptCenter.y-ptCur.y), (long double)(ptCur.x-m_ptCenter.x));

    return angle - pi/2.0;
}

void CRotationTracker::Draw(CDC * pOrgDC, CAdvImage * pImage, POINT * ppt)
{
    RECT    rect;
    CBitmap OrgBmp;
    CBitmap* oldBMOrg, * oldBM;

    BOOL bHilighted = FALSE;
    if(NULL != ppt)
    {
        m_ptIndicator.x = ppt->x + m_ptDiffIndi.x;
        m_ptIndicator.y = ppt->y + m_ptDiffIndi.y;

        bHilighted = TRUE;
    }

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

    double angle = GetRotationAngle(m_ptIndicator);
    CAdvImage * pTmpImage = pImage->Rotate(angle);
    if(NULL == pTmpImage)
    {
        ReleaseCapture();
        AfxMessageBox("Low memory!");
        return;
    }

    CBitmap * pBitmap = CreateBitmap(pDC, pTmpImage);
	BITMAP	bt;
	pBitmap->GetBitmap(&bt);

	CDC * MemDC = new CDC;
	MemDC->CreateCompatibleDC(pDC);
	oldBM = MemDC->SelectObject(pBitmap);
    int Width = pImage->GetWidth();
    int Height = pImage->GetHeight();
    POINT   ptLeftTop = GetLeftTop(Width>>1, Height>>1, angle);
	pDC->BitBlt(ptLeftTop.x, ptLeftTop.y, bt.bmWidth, bt.bmHeight, MemDC, 0, 0, SRCCOPY);
	MemDC->SelectObject(oldBM);
	delete MemDC;
    delete pBitmap;
    delete pTmpImage;

    rect.left  = m_ptIndicator.x - 5;
    rect.right = m_ptIndicator.x + 5;
    rect.top   = m_ptIndicator.y - 5;
    rect.bottom= m_ptIndicator.y + 5;
    HBRUSH  hBrush, hBrushOld;
    hBrush = CreateSolidBrush(bHilighted? RGB(0,255,0): RGB(0,125,0));
    hBrushOld = (HBRUSH) pDC->SelectObject(hBrush);
    pDC->Rectangle(&rect);
    pDC->SelectObject(hBrushOld);
    DeleteObject(hBrush);
    
    pDC->SelectObject(GetStockObject(WHITE_PEN));
    pDC->MoveTo(m_ptCenter);
    pDC->LineTo(m_ptIndicator);

	pOrgDC->BitBlt(0, 0, m_nViewWidth, m_nViewHeight, pDC, 0, 0, SRCCOPY);
    pDC->SelectObject(oldBMOrg);
    delete pDC;

    OrgBmp.DeleteObject();
    return;
}


POINT CRotationTracker::GetLeftTop(double CntrX, double CntrY, double angle)
{
    POINT   ptRet;

    double xt = CntrX * cos(angle);
    double yt = CntrY * sin(angle);

    int t[4];
    t[0] = round(-xt - yt);
    t[1] = round(-xt + yt);
    t[2] = round(xt + yt);
    t[3] = round(xt - yt);
    
    ptRet.x = t[0];
    for(int i=1; i<=3; i++)
    {
        if(ptRet.x > t[i])
            ptRet.x = t[i];
    }


    xt = CntrX * sin(angle);
    yt = CntrY * cos(angle);
    t[0] = round(-xt - yt);
    t[1] = round(-xt + yt);
    t[2] = round(xt + yt);
    t[3] = round(xt - yt);

    ptRet.y = t[0];
    for(int i=1; i<=3; i++)
    {
        if(ptRet.y < t[i])
            ptRet.y = t[i];
    }

    ptRet.x = m_ptCenter.x + ptRet.x;
    ptRet.y = m_ptCenter.y - ptRet.y;
    return ptRet;
}