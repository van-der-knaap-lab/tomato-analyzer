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
// PixelList.h: interface for the CPixelList class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable:4786)

#if !defined(AFX_PIXELLIST_H__C19888BB_BB15_49D2_9A39_744F2B401C4D__INCLUDED_)
#define AFX_PIXELLIST_H__C19888BB_BB15_49D2_9A39_744F2B401C4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <list>
#include "Pixel.h"

class CBoundingBox;
class CSliceImage;
class CAdvImage;

class CPixelList : public std::list<CPixel>
{
public:
	void Clear();
	CPixel GetAt(int index);
	CPixelList();
	CPixelList(const CPixelList & inList) : std::list<CPixel>(inList) { }
	virtual ~CPixelList() { }
//	bool operator<(const CPixelList &);
	bool operator==(const CPixelList & inList);

	//A very specific routine that will make a CSliceImage out of the pixel
	//	positions stored in this CPixelList structure.  Image will be psuedobinary.
	//	May change to binary or give option of specifying if should be binary.
	//	Up to user to clean up mem from image.
	//	The image will be normalized so that the upperleftmost pixel in the CPixelList
	//	will be (0, 0) in the new image, and the lower rightmost pixel in the CPixelLst
	//	will be (GetWidth()-1, GetHeight() -1) in the new image.
	CSliceImage * MakeImage();
    CAdvImage * MakeGrayScaleImage(void);
    CAdvImage * ContourToImage(void);
    void OffsetDec(POINT   ptOffset);

	//this routine will return the pixel that is the nearest approximation to the center
	//	of gravity of this binary object.
	CPixel GetGravityCenter();


	CBoundingBox GetBBox();
    void GetBoundingRect(RECT *  prt);

	BOOL IsIncluded(CPixel p, CPixel& pLocated, int nDistance);
	bool IsOnBoundary(int inWidth, int inHeight, CBoundingBox & inBBox);
	bool IsOnBoundary(int inWidth, int inHeight);

	void Normalize(CBoundingBox & inBBox);
	void Unnormalize(CBoundingBox & inBBox);

    void Serialize(CArchive& ar);

private:

protected:
	//Calculates the number of white pixels connected or surrounding inPix in the image
	//	inImage.
	int NumNeighbors(CPixel inPix, CSliceImage * inImage);

	//Helper function that safely checks if the pixel represented by (nx, ny) in inImage
	//	has the value val.
	bool PixHasVal( int nx, int ny, unsigned char val, CSliceImage * inImage);
};
#endif // !defined(AFX_PIXELLIST_H__C19888BB_BB15_49D2_9A39_744F2B401C4D__INCLUDED_)
