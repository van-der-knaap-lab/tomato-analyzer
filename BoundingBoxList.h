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
// BoundingBoxList.h: interface for the CBoundingBoxList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOUNDINGBOXLIST_H__23C68699_3334_4942_B5C0_2D26AA4CDC6E__INCLUDED_)
#define AFX_BOUNDINGBOXLIST_H__23C68699_3334_4942_B5C0_2D26AA4CDC6E__INCLUDED_

#include "Pixel.h"
#include "PixelList.h"
#include "BinObjs.h"
#include <vector>
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class CBoundingBox
{
	public:
		CRect GetRect();
		void  SetRect(CRect *);
		int GetWidth();

		int GetHeight();
		void Draw(CDC* pDC, float scale);
		CBoundingBox();
		CBoundingBox(const CBoundingBox & inBBox);
		CBoundingBox(CPixel mUpperLeft, CPixel mLowerRight);
		void ComputeBBox(CPixelList & inList);
		float GetLength();
		float GetArea();

		CPixel GetCenter();

		bool PointIn(CPoint inPoint);
		
        void Serialize(CArchive &ar);
		CPixel mUpperLeft, mLowerRight;
//		bool mState;
};


#endif // !defined(AFX_BOUNDINGBOXLIST_H__23C68699_3334_4942_B5C0_2D26AA4CDC6E__INCLUDED_)
