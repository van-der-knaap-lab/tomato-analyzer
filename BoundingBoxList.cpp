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
// BoundingBoxList.cpp: implementation of the CBoundingBoxList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "BoundingBoxList.h"
//#include "BinaryObjects.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void CBoundingBox::Serialize(CArchive &ar)
{
    if(ar.IsStoring())
    {
        ar << mUpperLeft.x << mUpperLeft.y << mLowerRight.x << mLowerRight.y;
    }
    else
    {
        ar >> mUpperLeft.x >> mUpperLeft.y >> mLowerRight.x >> mLowerRight.y;
    }
    return;
}

CBoundingBox::CBoundingBox()
{
	mUpperLeft.Set(0, 0);
	mLowerRight.Set(0, 0);
}

CBoundingBox::CBoundingBox(const CBoundingBox & inBBox)
{
	mUpperLeft = inBBox.mUpperLeft;
	mLowerRight = inBBox.mLowerRight;
}

CBoundingBox::CBoundingBox(CPixel mUpperLeft, CPixel mLowerRight)
{
	mUpperLeft = mUpperLeft;
	mLowerRight = mLowerRight;
}

void CBoundingBox::ComputeBBox(CPixelList & inList)
{
	if(inList.size() == 0)
		return;
	
	CPixelList::iterator pixIter = inList.begin();
	CPixel listPix = *pixIter;

	mUpperLeft = listPix;
	mLowerRight = listPix;
	
	pixIter++;
	while(pixIter != inList.end())
	{
		listPix = *pixIter;
		if(listPix.x < mUpperLeft.x)
			mUpperLeft.x = listPix.x;
		if(listPix.y < mUpperLeft.y)
			mUpperLeft.y = listPix.y;
		if(listPix.x > mLowerRight.x)
			mLowerRight.x = listPix.x;
		if(listPix.y > mLowerRight.y)
			mLowerRight.y = listPix.y;
		pixIter++;
	}

}

float CBoundingBox::GetLength()
{
	float y1, y2;
	y1 = (float)mUpperLeft.GetY();
	y2 = (float)mLowerRight.GetY();

	return y2 - y1;
	

}

float CBoundingBox::GetArea()
{
	float width, height;

	width = mLowerRight.x - mUpperLeft.x;
	height = mLowerRight.y - mUpperLeft.y;

	return width * height;
}

bool CBoundingBox::PointIn(CPoint inPoint)
{
	if((inPoint.x <= mLowerRight.x) && (inPoint.x >= mUpperLeft.x) &&
		(inPoint.y <= mLowerRight.y) && (inPoint.y >= mUpperLeft.y))
	{
		return true;
	}
	return false;

}

CPixel CBoundingBox::GetCenter()
{
	CPixel centerPix;

	centerPix.Set(mLowerRight.x + mUpperLeft.x, mLowerRight.y + mUpperLeft.y);
	centerPix.Set((float)centerPix.x/2.0f, (float)centerPix.y/2.0f);

	return centerPix;

}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


void CBoundingBox::Draw(CDC *pDC, float scale)
{
	CBrush Bbrush(RGB(0,255,255));
	pDC->FrameRect (CRect(	mUpperLeft.x*scale,
							mUpperLeft.y*scale,
							mLowerRight.x*scale,
							mLowerRight.y*scale), &Bbrush);
}

int CBoundingBox::GetHeight()
{
	return (mLowerRight.y - mUpperLeft.y);
}

int CBoundingBox::GetWidth()
{
	return (mLowerRight.x - mUpperLeft.x);
}

CRect CBoundingBox::GetRect()
{
	CRect rect = CRect(mUpperLeft.x,mUpperLeft.y,mLowerRight.x,mLowerRight.y);
	return rect;
}

void CBoundingBox::SetRect(CRect * p)
{
	mLowerRight.y = p->bottom;
	mLowerRight.x = p->right;

	mUpperLeft.y = p->top;
	mUpperLeft.x = p->left;
}
