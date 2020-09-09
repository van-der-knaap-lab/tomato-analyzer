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
// TriangleTracker.h: interface for the CTriangleTracker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TRIANGLETRACKER_H__2510012B_ADA0_43AA_8709_83BC11A059E4__INCLUDED_)
#define AFX_TRIANGLETRACKER_H__2510012B_ADA0_43AA_8709_83BC11A059E4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/** 
 * This class is used in manually revising the location of points. 
 */
class CTriangleTracker  
{
public:
	CTriangleTracker();
	virtual ~CTriangleTracker();

	/** 
	 * Provides an event loop that allows the user to select and drag a point (left mouse down/up) and
	 * signal that the user is done dragging (Enter/Escape). 
	 */
    BOOL Track(CView* pView, CBitmap * pBitmap, POINT * ppt);

	/**
	 * Returns 1 if the given point (representing the mouse click) is close enough to the selectable point 
	 * to select it, -1 otherwise. 
	 */
    int ClickOnIndicator(POINT pt);

    void Draw(CView * pView, CDC * pOrgDC, CBitmap * pBitmap);
    void GetResult(POINT *ppt);

private:
    int     m_nCurTrackedPoint;
    int     m_nViewWidth, m_nViewHeight;
    int     m_rectX, m_rectY;
    POINT   m_ptAngle[3];
    POINT   m_ptDiffIndi;
    POINT   m_ptLeftTop;
    bool    m_blOnlyTipMovable;
};

#endif // !defined(AFX_TRIANGLETRACKER_H__2510012B_ADA0_43AA_8709_83BC11A059E4__INCLUDED_)
