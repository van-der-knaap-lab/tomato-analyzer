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

#pragma once

class CShapeTracker
{
public:
	CShapeTracker(const CPixelList * lpSrcBoundary);
	~CShapeTracker();
	void Draw(CDC* pDC);
	void ShapeAdjust(int x, int y);
	BOOL Track(CView* pWnd, POINT, CWnd* pWndClipTo, CBitmap* pbp);
	BOOL IsShapeChanged(void);
	void DeleteSelectedPart(void);
	void DrawDelelteFlag(CDC* pDC, int, int);
	void Undo(void);
	void MakeShape(void);
	void FinalizeShapeTracking(void);
	CPixelList *  GetShape(void);
	void GetBinObj(CPixelList * pbin);
	//void GetNewBound(LPRECT rt);

private:
	int nClickNum;
	BOOL bChanged;
	CPixelList m_shape, m_add, m_line, m_backup;
	CPixelList m_BinObj;
	CPixel delFrom, delTo;
	CBitmap *	m_pBitmap;
	//RECT	rtBound;

};