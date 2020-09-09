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
/* EllipseTracker
 * Places the left and top points of the ellipse on the tomato
 * Allows them to be user-adjusted
 * Resizes and Redraws the ellipse based on those two points
 * Places Center point
 * Allows whole ellipse to be moved based on center point
 */

#pragma once

class CEllipseTracker
{
public:
	CEllipseTracker();
	~CEllipseTracker();
	void Draw(CView* pView, CDC * pOrgDC, CBitmap * pBitmap);
	BOOL Track(CView * view, CBitmap * bp, POINT * pt);
	int ClickOnIndicator(POINT pt);
	void GetResult(POINT * ppt);

private:
	POINT  ellipse_points[3];
	int     m_nViewWidth, m_nViewHeight;
    bool    m_blOnlyTipMovable;
    int     m_nCurTrackedPoint;
	int     m_rectX, m_rectY;
    POINT   m_ptDiffIndi;
    POINT   m_ptLeftTop;
};