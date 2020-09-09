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
// PeriTracker.cpp: implementation of the PeriTracker class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "resource.h"
#include "PixelList.h"
#include "SliceImage.h"

#include "AdvImage.h"

#include <vector>
using namespace std;
#include "new_img_Function.h"
#include "PeriTracker.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern int findPointPosition(CPixelList * pCntr, int x, int y);

PeriTracker::PeriTracker(const CPixelList * lpSrcBoundary)
{
	nClickNum = 0;

	bChanged = FALSE;

	m_pBitmap = NULL;

	delFrom.Set(-1, -1);
	delTo.Set(-1, -1);

	m_shape.assign(lpSrcBoundary->begin(), lpSrcBoundary->end());
	m_backup.assign(lpSrcBoundary->begin(), lpSrcBoundary->end());

}

PeriTracker::~PeriTracker()
{
}

/*
void PeriTracker::GetNewBound(LPRECT prt)
{
prt->bottom = rtBound.bottom;
prt->left   = rtBound.left;
prt->right  = rtBound.right;
prt->top    = rtBound.top;
}
*/

void PeriTracker::Draw(CDC* pDC)
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


void PeriTracker::DrawDelelteFlag(CDC* pDC, int x, int y)
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
void PeriTracker::ShapeAdjust(int x, int y)
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

// we changed this method to delete using indices rather than a boolean flag because it wasn't working correctly
// and kept deleting random parts and never deleted the right parts between the delete flags
// Last edited: 6/21/2015
void PeriTracker::DeleteSelectedPart()
{
	CPixelList pxlstA, pxlstB;  // store pixels between two delete flags
	m_org = m_shape;

	BOOL containsZero = false;


	CPixelList * temporary = &m_shape;

	index1 = findPointPosition(temporary, delFrom.x, delFrom.y);  // index of deleteFrom
	index2 = findPointPosition(temporary, delTo.x, delTo.y);  // index of deleteTo
	int length = m_shape.size();
	m_included = false;

	CPixelList tempShape;
	if (index1 > index2)
	{
		for (int i = index2; i <= index1; i++)
		{
			pxlstA.push_back(m_shape.GetAt(i));
			m_included = true;
		}
		for (int i = index1; i != index2 + 1; i++)
		{
			if (i == length)
			{
				i = 0;
			}
			pxlstB.push_back(m_shape.GetAt(i));
		}
	}
	else
	{
		for (int i = index1; i <= index2; i++)
		{
			pxlstA.push_back(m_shape.GetAt(i));
			m_included = true;
		}
		for (int i = index2; i != index1 + 1; i++)
		{
			if (i == length)
				i = 0;

			pxlstB.push_back(m_shape.GetAt(i));
		}
	}

	int sizeA = pxlstA.size();
	int sizeB = pxlstB.size();
	if (sizeA > sizeB)
	{
		CPixelList tempA;
		//for (int l = sizeA - 1; l >= 0; l--)  // modified on 6/22/2015
		for (int l = 0; l < sizeA; l++)
		{
			tempA.push_back(pxlstA.GetAt(l));
		}
		m_shape.assign(tempA.begin(), tempA.end());
	}
	else
	{
		CPixelList tempB;
		//for (int p = sizeB - 1; p >= 0; p--)  // modified on 6/22/2015
		for (int p = 0; p < sizeB; p++)
		{
			tempB.push_back(pxlstB.GetAt(p));
		}
		m_shape.assign(tempB.begin(), tempB.end());
	}
	return;
}

void PeriTracker::Undo()
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

/** (written on 6/22/2015)
* Constructs a new line between two delete points when the pericarp boundary is being adjusted.
* - m_line stores the new line.
* - m_shape stores the remaining pericarp boundary after the user has deleted part of the pericarp boundary.
* - m_add stores the adjusting point(s) the user selects. m_add is populated in ShapeAdjust(), this is also
*   called from ShapeAdjust(), and from Undo()
*/
void PeriTracker::MakeShape()
{
	if (!m_line.empty())		// if pixel list has stuff in it, clear it
		m_line.clear();

	int nLength = m_add.size();
	if (nLength < 1)
		return;

	CPixelList * pline;		// a buffer pixel list to store the pixels between two adjoining points

	CPixelList delFromSegment;  // stores the line created between the delFrom point and the first adjusting point
	CPixelList delToSegment;  // stores the line created between the delTo point and the last adjusting point
	CPixelList midSegments;  // stores lines that link multiple adjusting points together

	POINT p1, p2, p3;
	CPixelList::iterator Iter = m_add.begin();
	CPixelList::iterator IterJ;

	/************************** Populate delFromSegment & delToSegment *************************/
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
	if ((p1.x == delFrom.x) && (p1.y == delFrom.y)) // if p1 == delFrom, push pline to delFromSegment
	{
		for (IterJ = pline->begin(); IterJ != pline->end(); IterJ++)
			delFromSegment.push_back(*IterJ);
	}
	else if ((p1.x == delTo.x) && (p1.y == delTo.y))  // if p1 == delTo, push pline to delToSegment
	{
		for (IterJ = pline->begin(); IterJ != pline->end(); IterJ++)
			delToSegment.push_back(*IterJ);
	}
	delete pline;

	CPixelList::reverse_iterator Iterrev = m_add.rbegin();
	p3.x = Iterrev->x;
	p3.y = Iterrev->y;
	pline = MakeLine(p2, p3);
	if ((p2.x == delFrom.x) && (p2.y == delFrom.y))  // if p2 == delFrom, push pline to delFromSegment
	{
		for (IterJ = pline->begin(); IterJ != pline->end(); IterJ++)
			delFromSegment.push_back(*IterJ);
	}
	else if ((p2.x == delTo.x) && (p2.y == delTo.y))  // if p2 == delTo, push pline to delToSegment
	{
		for (IterJ = pline->begin(); IterJ != pline->end(); IterJ++)
			delToSegment.push_back(*IterJ);
	}
	delete pline;

	/*************************** Populate midSegments ****************************/
	Iter = m_add.begin();
	CPixelList::iterator IterPrev = Iter;
	for (int i = 1; i<nLength; i++) // for the part that was adjusted
	{
		p1.x = IterPrev->x;
		p1.y = IterPrev->y;
		++Iter;
		p2.x = Iter->x;
		p2.y = Iter->y;

		pline = MakeLine(p1, p2);	// make a line between them
		for (IterJ = pline->begin(); IterJ != pline->end(); IterJ++)  // push pline to midSegments
			midSegments.push_back(*IterJ);

		IterPrev = Iter;
		delete pline;
	}

	/********************* Sort the segments in order and push back to m_line ************************/
	if (abs(m_shape.GetAt(0).x - delFrom.x) <= 2 && abs(m_shape.GetAt(0).y - delFrom.y) <= 2)  // if m_shape starts with delFrom
	{
		for (CPixelList::iterator Iter = delToSegment.begin(); Iter != delToSegment.end(); Iter++)  // push delToSegment first
			m_line.push_back(*Iter);

		if (nLength > 1)  // if midSegments is not empty, push it to m_line
		{
			if (midSegments.GetAt(0) == m_line.GetAt(m_line.size() - 1))
			{
				for (int index = 1; index < midSegments.size(); index++)  // cut off the repeated pixel
					m_line.push_back(midSegments.GetAt(index));
			}
			else if (midSegments.GetAt(midSegments.size() - 1) == m_line.GetAt(m_line.size() - 1))
			{
				for (int index = midSegments.size() - 2; index >= 0; index--)  // cut off the repeated pixel
					m_line.push_back(midSegments.GetAt(index));
			}
		}

		for (CPixelList::reverse_iterator Iterrev = delFromSegment.rbegin(); Iterrev != delFromSegment.rend(); Iterrev++)  // reverse delFromSegment and push it
			m_line.push_back(*Iterrev);
	}
	else if (abs(m_shape.GetAt(0).x - delTo.x) <= 2 && abs(m_shape.GetAt(0).y - delTo.y) <= 2)  // if m_shape starts with delTo
	{
		for (CPixelList::iterator Iter = delFromSegment.begin(); Iter != delFromSegment.end(); Iter++)  // push delFromSegment first
			m_line.push_back(*Iter);

		if (nLength > 1)  // if midSegments is not empty, push it to m_line
		{
			if (midSegments.GetAt(0) == m_line.GetAt(m_line.size() - 1))
			{
				for (int index = 1; index < midSegments.size(); index++)  // cut off the repeated pixel
					m_line.push_back(midSegments.GetAt(index));
			}
			else if (midSegments.GetAt(midSegments.size() - 1) == m_line.GetAt(m_line.size() - 1))
			{
				for (int index = midSegments.size() - 2; index >= 0; index--)  // cut off the repeated pixel
					m_line.push_back(midSegments.GetAt(index));
			}
		}

		for (CPixelList::reverse_iterator Iterrev = delToSegment.rbegin(); Iterrev != delToSegment.rend(); Iterrev++)  // reverse delToSegment and push it
			m_line.push_back(*Iterrev);
	}
	return;
}

BOOL PeriTracker::IsShapeChanged()
{
	return bChanged;
}


void PeriTracker::GetBinObj(CPixelList * pbin)
{
	pbin->assign(m_BinObj.begin(), m_BinObj.end());
}


CPixelList * PeriTracker::GetShape()
{
	return &m_line;
}


void PeriTracker::FinalizeShapeTracking()
{
	bChanged = !m_add.empty();
	if (bChanged)
	{
		// Last edited: 6/22/2015
		/****** Check if there is repeated pixel at the endpoint of m_shape and remove it *****/
		CPixelList cleanedRemainBoundary;
		int begin = (m_shape.GetAt(0) == m_line.GetAt(m_line.size() - 1)) ? 1 : 0;
		int end = (m_shape.GetAt(m_shape.size() - 1) == m_line.GetAt(0)) ? m_shape.size() - 1 : m_shape.size();
		for (begin; begin < end; begin++)
			cleanedRemainBoundary.push_back(m_shape.GetAt(begin));
		m_shape = cleanedRemainBoundary;

		/***** Push m_shape to m_line *****/
		CPixelList::iterator Iter;
		CPixelList::iterator IterEnd = m_shape.end();
		for (Iter = m_shape.begin(); Iter != IterEnd; ++Iter)
			m_line.push_back(*Iter);
	}
	return;
}


BOOL PeriTracker::Track(CView* pWnd, CWnd* pWndClipTo, CBitmap* pbp)
{
	if (::GetCapture() != NULL)
		return FALSE;

	m_pBitmap = pbp;
	m_firstTime = true;

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
