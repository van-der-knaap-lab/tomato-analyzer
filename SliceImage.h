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
// SliceImage.h: interface for the CSliceImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SLICEIMAGE_H__A5117985_A2A9_4565_99A2_1C32A080E28F__INCLUDED_)
#define AFX_SLICEIMAGE_H__A5117985_A2A9_4565_99A2_1C32A080E28F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AdvImage.h"
#include "BoundingBoxList.h"
#include "Pixel.h"
#include "PixelList.h"

class CSliceImage : public CAdvImage  
{
public:
	int GetArea();
	void CutImageByLine(CPixel p1, CPixel p2, BOOL flag=0);
	CSliceImage();
	virtual ~CSliceImage();

	CSliceImage * GetCopy();

	//this CSlice image is such that all pixels that make up a slice have a red
	//	value of one.  The background is black.  See implementation for algorithm
	//	details.

	unsigned char BandValueAt(unsigned int band, int xcoordinate, int ycoordinate, bool inGlobal = true);
	void SetBandVal(unsigned int band, int xcoord, int ycoord, unsigned char inval, bool inGlobal = true);

	//Turns all black pixels white and white pixels black.
	void Invert();


	CBinObjs * GetBinaryObjects();
	//Some algorithms on a CSliceImage need an arbitrary pixel to start from
	//	if this pixel is not (-1, -1), then it is such a pixel.

	//Overriding color binary objects for this class.  This routine should
	//	use the global bounding box to normalize all of the pixels in inList
	//	before setting their value.
	void ColorBinaryObject(CPixelList & inList, float inr, float ing, float inb);


	bool IsBlack(int x, int y);
	bool IsBoundaryPixel(CPixel & inPix);	
		//End code

	CSliceImage * ThinImage();

	CPixel mStart;
	CPixel m_Bottom;
	CBoundingBox mGlobalBBox;
};

#endif // !defined(AFX_SLICEIMAGE_H__A5117985_A2A9_4565_99A2_1C32A080E28F__INCLUDED_)
