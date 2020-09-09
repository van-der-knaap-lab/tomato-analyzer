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

#include "resource.h"
#include "PixelList.h"
#include "TrackShape.h"
#include "SliceImage.h"

#include "AdvImage.h"

#include <vector>
using namespace std;
#include "new_img_Function.h"


CShapeTracker::CShapeTracker(const CPixelList * lpSrcBoundary)
{
	nClickNum = 0;

	bChanged = FALSE;

	m_pBitmap = NULL;

	delFrom.Set(-1, -1);
	delTo.Set(-1, -1);

	m_shape.assign(lpSrcBoundary->begin(), lpSrcBoundary->end());
	m_backup.assign(lpSrcBoundary->begin(), lpSrcBoundary->end());
}


CShapeTracker::~CShapeTracker()
{
}

/*
void CShapeTracker::GetNewBound(LPRECT prt)
{
prt->bottom = rtBound.bottom;
prt->left   = rtBound.left;
prt->right  = rtBound.right;
prt->top    = rtBound.top;
}
*/

void CShapeTracker::Draw(CDC* pDC)
{
	if (m_shape.size() == 0)
		return;

	int dcIndex = pDC->SaveDC();
	pDC->SetMapMode(MM_TEXT);

	BITMAP bm;
	m_pBitmap->GetBitmap(&bm);

	CDC dcTmp;
	dcTmp.CreateCompatibleDC(pDC);
	CBitmap * poldbm = dcTmp.SelectObject(m_pBitmap);
	pDC->BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcTmp, 0, 0, SRCCOPY);
	dcTmp.SelectObject(poldbm);

	pDC->RestoreDC(dcIndex);

	CPixelList::iterator myIter;
	CPixelList::iterator IterEnd = m_shape.end();
	for (myIter = m_shape.begin(); myIter != IterEnd; ++myIter)
	{
		pDC->SetPixel(myIter->x, myIter->y, RGB(255, 255, 0));
	}

	if (delFrom.x >= 0)
		DrawDelelteFlag(pDC, delFrom.x, delFrom.y);

	if (delTo.x >= 0)
		DrawDelelteFlag(pDC, delTo.x, delTo.y);

	if (!m_add.empty())
	{
		IterEnd = m_line.end();
		for (myIter = m_line.begin(); myIter != IterEnd; ++myIter)
		{
			pDC->SetPixel(myIter->x, myIter->y, RGB(0, 255, 255));
		}
	}

}


void CShapeTracker::DrawDelelteFlag(CDC* pDC, int x, int y)
{
	CBitmap bp;
	BITMAP	bm;

	bp.LoadBitmap(IDB_DELETE_FLAG);
	bp.GetBitmap(&bm);

	CDC * MemDC = new CDC;
	MemDC->CreateCompatibleDC(pDC);
	CBitmap* oldBM = MemDC->SelectObject(&bp);

	pDC->BitBlt(x - bm.bmWidth / 2, y - bm.bmHeight / 2, bm.bmWidth, bm.bmHeight, MemDC, 0, 0, SRCCOPY);

	MemDC->SelectObject(oldBM);
	delete MemDC;
}

// this is where the m_add pixel list is populated, it is where the pixel list that is used in makeShape is populated
void CShapeTracker::ShapeAdjust(int x, int y)
{
	CPixel pxl(x, y);
	CPixel pxlLocated(0, 0);

	if (nClickNum == 0)
	{
		if (m_shape.IsIncluded(pxl, pxlLocated, 4))
		{
			delFrom = pxlLocated;
			nClickNum++;
		}
	}
	else if (nClickNum == 1)
	{
		if (m_shape.IsIncluded(pxl, pxlLocated, 4))
		{
			delTo = pxlLocated;
			nClickNum++;

			DeleteSelectedPart();
		}
	}
	else if (nClickNum == 2)
	{
		m_add.push_back(pxl);

		MakeShape();
	}

	return;
}


void CShapeTracker::DeleteSelectedPart()
{
	CPixelList pxlstA, pxlstB;

	BOOL bAssignA = TRUE;

	CPixelList::iterator myIter;
	for (myIter = m_shape.begin(); myIter != m_shape.end(); myIter++)
	{
		CPixel pixel = *myIter;
		if (pixel == delFrom || pixel == delTo)
		{
			bAssignA = !bAssignA;
		}

		if (bAssignA)
			pxlstA.push_back(pixel);
		else
			pxlstB.push_back(pixel);
	}

	int nLenA = pxlstA.size();
	int nLenB = pxlstB.size();
	if (nLenA>nLenB)
		m_shape.assign(pxlstA.begin(), pxlstA.end());
	else
		m_shape.assign(pxlstB.begin(), pxlstB.end());

	return;
}


void CShapeTracker::Undo()
{
	int nSize = m_add.size();
	if (nSize == 0)
	{
		if (nClickNum == 2)
		{
			delTo.Set(-1, -1);
			nClickNum = 1;

			m_shape.assign(m_backup.begin(), m_backup.end());
		}
		else if (nClickNum == 1)
		{
			delFrom.Set(-1, -1);
			nClickNum = 0;
		}
	}
	else
	{
		if (nSize > 1)
			m_add.pop_back();
		else if (nSize == 1)
			m_add.clear();

		MakeShape();
	}

	return;
}

// m_add is populated in ShapeAdjust(), this is also called from ShapeAdjust(), and from Undo()
void CShapeTracker::MakeShape()
{
	if (!m_line.empty())		// if pixel list has stuff in it, clear it
		m_line.clear();

	int nLength = m_add.size();
	if (nLength < 1)
		return;

	CPixelList * pline;		// make another pixel list to store the points on the line

	CPixelList::iterator Iter = m_add.begin();
	CPixelList::iterator IterPrev = Iter;
	CPixelList::iterator IterJ;

	POINT p1, p2, p3;
	for (int i = 1; i<nLength; i++) // for the part that was adjusted
	{
		p1.x = IterPrev->x;
		p1.y = IterPrev->y;
		++Iter;
		p2.x = Iter->x;
		p2.y = Iter->y;

		pline = MakeLine(p1, p2);	// make a line between them

		for (IterJ = pline->begin(); IterJ != pline->end(); IterJ++)
			m_line.push_back(*IterJ);	// push back the points on the line into the boundary list

		IterPrev = Iter;
		delete pline;
	}

	Iter = m_add.begin();
	int nx = Iter->x - delFrom.x;	// delFrom = the first point the user clicks when redoing the boundary
	int ny = Iter->y - delFrom.y;
	int nd1 = nx*nx + ny*ny;

	nx = Iter->x - delTo.x;			// delTo = the second point the user clicks when redoing the boundary
	ny = Iter->y - delTo.y;
	int nd2 = nx*nx + ny*ny;
	if (nd1 < nd2)
	{
		p1.x = delFrom.x;
		p1.y = delFrom.y;
		p2.x = delTo.x;
		p2.y = delTo.y;
	}
	else
	{
		p2.x = delFrom.x;
		p2.y = delFrom.y;
		p1.x = delTo.x;
		p1.y = delTo.y;
	}

	p3.x = Iter->x;
	p3.y = Iter->y;
	pline = MakeLine(p1, p3);
	for (IterJ = pline->begin(); IterJ != pline->end(); IterJ++)
		m_line.push_back(*IterJ);
	delete pline;

	CPixelList::reverse_iterator	Iterrev = m_add.rbegin();
	p3.x = Iterrev->x;
	p3.y = Iterrev->y;
	pline = MakeLine(p2, p3);
	for (IterJ = pline->begin(); IterJ != pline->end(); IterJ++)
		m_line.push_back(*IterJ);
	delete pline;

	m_line.push_back(CPixel(p3.x, p3.y));
	return;
}

BOOL CShapeTracker::IsShapeChanged()
{
	return bChanged;
}


void CShapeTracker::GetBinObj(CPixelList * pbin)
{
	pbin->assign(m_BinObj.begin(), m_BinObj.end());
}


CPixelList * CShapeTracker::GetShape()
{
	return &m_line;
}


void CShapeTracker::FinalizeShapeTracking()
{
	bChanged = !m_add.empty();
	if (bChanged)
	{
		CPixelList::iterator Iter;
		CPixelList::iterator IterEnd = m_shape.end();
		for (Iter = m_shape.begin(); Iter != IterEnd; ++Iter)
			m_line.push_back(*Iter);
	}
	return;
}


BOOL CShapeTracker::Track(CView* pWnd, POINT Offset, CWnd* pWndClipTo, CBitmap* pbp)
{
	if (::GetCapture() != NULL)
		return FALSE;

	m_pBitmap = pbp;

	// set capture to the window which received this message
	pWnd->SetCapture();
	ASSERT(pWnd == CWnd::GetCapture());

	if (pWndClipTo != NULL)
		pWndClipTo->UpdateWindow();

	// get DC for drawing
	CDC* pDrawDC;
	if (pWndClipTo != NULL)
	{
		// clip to arbitrary window by using adjusted Window DC
		pDrawDC = pWndClipTo->GetDCEx(NULL, DCX_CACHE);
	}
	else
	{
		// otherwise, just use normal DC
		pDrawDC = pWnd->GetDC();
	}
	ASSERT_VALID(pDrawDC);

	pWnd->OnPrepareDC(pDrawDC, NULL);
	Draw(pDrawDC);

	// get messages until capture lost or cancelled/accepted
	BOOL bContinue = TRUE;
	BOOL bConfirm = FALSE;
	while (bContinue)
	{
		MSG msg;
		VERIFY(::GetMessage(&msg, NULL, 0, 0));

		if (CWnd::GetCapture() != pWnd)
			break;

		POINT   pt;
		switch (msg.message)
		{
		case WM_LBUTTONDOWN:
			pt.x = (int)LOWORD(msg.lParam);  pt.y = (int)HIWORD(msg.lParam);
			pDrawDC->DPtoLP(&pt);
			ShapeAdjust(pt.x, pt.y);
			Draw(pDrawDC);
			break;

			// handle cancel messages
		case WM_RBUTTONDOWN:
			Undo();
			Draw(pDrawDC);
			break;

		case WM_RBUTTONUP:
			break;

		case WM_KEYDOWN:
			if (msg.wParam == VK_ESCAPE)
				bContinue = FALSE;
			else if (msg.wParam == VK_RETURN)
			{
				bContinue = FALSE;
				FinalizeShapeTracking();
			}
			break;

			// just dispatch rest of the messages
		default:
			DispatchMessage(&msg);
			break;
		}
	}

	if (pWndClipTo != NULL)
		pWndClipTo->ReleaseDC(pDrawDC);
	else
		pWnd->ReleaseDC(pDrawDC);

	ReleaseCapture();

	// return TRUE only if rect has changed
	return bChanged;
}


//DSTINVERT