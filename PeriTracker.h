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
// PeriTracker.h: interface for the PeriTracker class.
//
//////////////////////////////////////////////////////////////////////

//#if !defined(AFX_PERITRACKER_H__E9E56549_0647_446C_B2C5_61DEFD754285__INCLUDED_)
//#define AFX_PERITRACKER_H__E9E56549_0647_446C_B2C5_61DEFD754285__INCLUDED_

//#if _MSC_VER > 1000
#pragma once
//#endif // _MSC_VER > 1000

class PeriTracker
{
public:
	PeriTracker(const CPixelList * lpSrcBoundary);
	~PeriTracker();
	void Draw(CDC* pDC);
	void ShapeAdjust(int x, int y);
	BOOL Track(CView* pWnd, /*POINT, */CWnd* pWndClipTo, CBitmap* pbp);
	BOOL IsShapeChanged(void);
	void DeleteSelectedPart(void);
	void DrawDelelteFlag(CDC* pDC, int, int);
	void Undo(void);
	void MakeShape(void);
	void FinalizeShapeTracking(void);
	CPixelList *  GetShape(void);
	void GetBinObj(CPixelList * pbin);
	void GetNewBound(LPRECT rt);

private:
	int nClickNum;
	BOOL bChanged;
	CPixelList m_shape, m_add, m_line, m_backup;
	CPixelList m_BinObj;
	CPixelList m_org;
	CPixel delFrom, delTo;
	CBitmap *	m_pBitmap;
	RECT rtBound;
	int index1, index2;
	bool m_included;
	bool m_firstTime;
};

//#endif // !defined(AFX_PERITRACKER_H__E9E56549_0647_446C_B2C5_61DEFD754285__INCLUDED_)
