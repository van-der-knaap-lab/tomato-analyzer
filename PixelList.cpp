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
// PixelList.cpp: implementation of the CPixelList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "SliceImage.h"
#include "PixelList.h"
#include "CVIPdecls.h"
#include "BoundingBoxList.h"
#include <assert.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPixelList::CPixelList() : std::list<CPixel>()
{

}
  

bool CPixelList::operator==(const CPixelList & inList)
{
	return false;
}


int CPixelList::NumNeighbors(CPixel inPix, CSliceImage * inImage)
{
	int numNeighbors = 0;

	//Normalizing the pixel.
	if( !inImage)
		return 0;
	inPix.x -= inImage->mGlobalBBox.mUpperLeft.GetX();
	inPix.y -= inImage->mGlobalBBox.mUpperLeft.GetY();

	if(PixHasVal(inPix.x-1, inPix.y-1, 255, inImage))
		numNeighbors++;
	if(PixHasVal(inPix.x-1, inPix.y, 255, inImage))
		numNeighbors++;
	if(PixHasVal(inPix.x, inPix.y-1, 255, inImage))
		numNeighbors++;
	if(PixHasVal(inPix.x+1, inPix.y+1, 255, inImage))
		numNeighbors++;
	if(PixHasVal(inPix.x+1, inPix.y, 255, inImage))
		numNeighbors++;
	if(PixHasVal(inPix.x, inPix.y+1, 255, inImage))
		numNeighbors++;
	if(PixHasVal(inPix.x-1, inPix.y+1, 255, inImage))
		numNeighbors++;
	if(PixHasVal(inPix.x+1, inPix.y-1, 255, inImage))
		numNeighbors++;
		
	return numNeighbors;	

}

bool CPixelList::PixHasVal( int nx, int ny, unsigned char val, CSliceImage * inImage)
{
	unsigned char theval;
	if((nx >= 0) && (ny >= 0) && 
		(nx < inImage->GetWidth()) &&
		(ny < inImage->GetHeight()))
	{
		theval = inImage->mImage->image_ptr[0]->rptr[ny][nx];
		if(theval == val)
			return true;
		else
			return false;	
	}
	return false;
	
}

CAdvImage * CPixelList::MakeGrayScaleImage()
{
	if(this->size() <= 1)
        return NULL;

	CPixelList::iterator MyIter, IterEnd;
	CPixel mLowerRight, mUpperLeft, pix;

	MyIter = this->begin();
    IterEnd = this->end();

	mLowerRight = *(MyIter);
	mUpperLeft = mLowerRight;
	++MyIter;

	for(; MyIter!=IterEnd; ++MyIter)
	{
		if(MyIter->x < mUpperLeft.x)
			mUpperLeft.x = MyIter->x;
        else if(MyIter->x > mLowerRight.x)
        {
			mLowerRight.x = MyIter->x;
        }

		if(MyIter->y < mUpperLeft.y)
			mUpperLeft.y = MyIter->y;
        else if(MyIter->y > mLowerRight.y)
        {
			mLowerRight.y = MyIter->y;
        }
	}

    CAdvImage * outImage = new CAdvImage;
    outImage->CreateGrayScale(mLowerRight.x - mUpperLeft.x +1,
			                  mLowerRight.y - mUpperLeft.y +1
                             );
    if(NULL == outImage->mImage)
    {
        delete outImage;
        return NULL;
    }

	unsigned char ** rBand = outImage->GetBand(0);
	for(MyIter = this->begin(); MyIter!=IterEnd; ++MyIter)
	{
		pix = *MyIter;

		//To effectively deal with the pixel list offsets.
		pix.x -= mUpperLeft.x;
		pix.y -= mUpperLeft.y;

		rBand[pix.y][pix.x] = 255;
	}

	return outImage;
}

CSliceImage * CPixelList::MakeImage()
{
	//DECLARATIONS
	CBoundingBox picBounds;
	int i, j; //loop counters
	CSliceImage * outImage = new CSliceImage();
	int size = this->size();
	CPixelList::iterator iter;
	CPixel pix, startpix;
	int rows, cols;

	//Check that size is greater than 1 or something stupid like that.
	if(this->size() <= 1)
	{
		delete outImage;
		return NULL;
	}
	startpix = this->front();
	picBounds.ComputeBBox(*this);
	startpix.x -=(picBounds.mUpperLeft.x);
	startpix.y -=(picBounds.mUpperLeft.y);
	outImage->mGlobalBBox = picBounds;

	//Creating new image to return;
	outImage->mImage = new_Image(PPM, RGB, 3, 
			picBounds.mLowerRight.GetY() - picBounds.mUpperLeft.GetY()+1,
			picBounds.mLowerRight.GetX() - picBounds.mUpperLeft.GetX()+1,
			CVIP_BYTE, REAL);


	rows = getNoOfRows_Image(outImage->mImage);
	cols = getNoOfCols_Image(outImage->mImage);

	unsigned char ** rBand = outImage->GetBand(0);
	unsigned char ** gBand = outImage->GetBand(1);
	unsigned char ** bBand = outImage->GetBand(2);

	//Every pixel is black initially
	for(i = 0; i < rows; i++)
		for(j = 0; j < cols; j++)
		{
			rBand[i][j] = gBand[i][j] = bBand[i][j] = 0;
		}

	//Those pixels in the list become white.
	for(iter = this->begin(); iter != this->end(); iter++)
	{
		pix = *iter;
		CPixel oldPix = pix;

		//To effectively deal with the pixel list offsets.
		pix.x -= (picBounds.mUpperLeft.x);
		pix.y -= (picBounds.mUpperLeft.y);

		rBand[pix.y][pix.x] = 255;
		gBand[pix.y][pix.x] = 255;
		bBand[pix.y][pix.x] = 255;
	}
	outImage->mStart = startpix;

	return outImage;

}


CPixel CPixelList::GetGravityCenter()
{
	CPixel gCenter;

	int numPixelsInObj;
	float sumXs = 0.0f, sumYs = 0.0f;

	numPixelsInObj = this->size();
	assert(numPixelsInObj != 0);

	CPixelList::iterator myIter;

	//Summing x and y coordinates.
	for(myIter = this->begin(); myIter != this->end(); myIter++)
	{
		sumXs += (float)(*myIter).x;
		sumYs += (float)(*myIter).y;
	}
	sumXs /= (float)numPixelsInObj;
	sumYs /= (float)numPixelsInObj;

	gCenter.x = sumXs;
	gCenter.y = sumYs;

	return gCenter;
}



//Will return whether any pixel in the list falls on the boundaries.
//	May be passing in a bounding box.
BOOL CPixelList::IsIncluded(CPixel p, CPixel& pLocated, int nDistance)
{
	int  nd, x, y;
	BOOL bFound = FALSE;
	int  nmind = (nDistance+1)*(nDistance+1);

	CPixelList::iterator myIter;
	for(myIter = begin ();myIter!=end();myIter++)
	{
		x = myIter->x - p.x;
		y = myIter->y - p.y;
		nd = x*x + y*y;
		if(nd < nmind)
		{
			pLocated.Set(myIter->x, myIter->y);
			if(nd == 0)
				return TRUE;
			
			nmind = nd;
			bFound = TRUE;
		}
	}

	return bFound;
}

bool CPixelList::IsOnBoundary(int inWidth, int inHeight)
{
	CPixelList::iterator myIter;
	CPixel pix;

	myIter = this->begin();
	
	while(myIter != this->end())
	{
		pix = *myIter;
		if((pix.x == 0) || (pix.y == 0) || (pix.x == (inWidth - 1)) || (pix.y == (inHeight - 1)))
		{
			return true;
		}
		myIter++;
	}

	return false;
}


//Will return whether any pixel in the list falls on the boundaries.
//	May be passing in a bounding box.
bool CPixelList::IsOnBoundary(int inWidth, int inHeight, CBoundingBox & inBBox)
{
	CPixelList::iterator myIter;
	CPixel pix;

	myIter = this->begin();
	
	while(myIter != this->end())
	{
		pix = *myIter;
		pix.x -= inBBox.mUpperLeft.x;
		pix.y -= inBBox.mUpperLeft.y;

		if((pix.x == 0) || (pix.y == 0) || (pix.x == (inWidth - 1)) || (pix.y == (inHeight - 1)))
		{
			return true;
		}
		myIter++;
	}

	return false;
}

CBoundingBox CPixelList::GetBBox()
{
	CPixelList::iterator myIter;
	CBoundingBox outBox;
	CPixel pix;

	myIter = this->begin();

	outBox.mLowerRight = *(myIter);
	outBox.mUpperLeft = outBox.mLowerRight;
	myIter++;

	for(; myIter != this->end(); myIter++)
	{
		pix = *myIter;

		if(pix.x < outBox.mUpperLeft.x)
			outBox.mUpperLeft.x = pix.x;
		if(pix.y < outBox.mUpperLeft.y)
			outBox.mUpperLeft.y = pix.y;
		if(pix.x > outBox.mLowerRight.x)
			outBox.mLowerRight.x = pix.x;
		if(pix.y > outBox.mLowerRight.y)
			outBox.mLowerRight.y = pix.y;	

	}

	return outBox;
}


void CPixelList::GetBoundingRect(RECT *  prt)
{
	CPixelList::iterator pIter = begin();
    CPixelList::iterator pIterEnd = end();

    prt->left = prt->right = pIter->x;
    prt->top = prt->bottom = pIter->y;
    
    while((++pIter) != pIterEnd)
    {
		if(pIter->x < prt->left)
            prt->left = pIter->x;
        else if(pIter->x > prt->right)
            prt->right= pIter->x;

		if(pIter->y < prt->top)
            prt->top = pIter->y;
        else if(pIter->y > prt->bottom)
            prt->bottom = pIter->y;
    }

	return;
}

void CPixelList::OffsetDec(POINT   ptOffset)
{
	CPixelList::iterator pIter = begin();
    CPixelList::iterator pIterEnd = end();
    while(pIter != pIterEnd)
    {
        pIter->x -= ptOffset.x;
        pIter->y -= ptOffset.y;
    }

    return;
}

void CPixelList::Normalize(CBoundingBox & inBBox)
{
	CPixelList::iterator pIter;
	CPixel pix;

	for(pIter = begin(); pIter != end(); pIter++)
	{
		pix = *pIter;
		pix.Set(pix.x + inBBox.mUpperLeft.x, pix.y + inBBox.mUpperLeft.y);
		*pIter = pix;
	}

}

void CPixelList::Unnormalize(CBoundingBox & inBBox)
{
	CPixelList::iterator pIter;
	CPixel pix;

	for(pIter = begin(); pIter != end(); pIter++)
	{
		pix = *pIter;
		pix.Set(pix.x - inBBox.mUpperLeft.x, pix.y - inBBox.mUpperLeft.y);
		*pIter = pix;
	}
}


CPixel CPixelList::GetAt(int index)
{
	CPixelList::iterator pIter;
	CPixel pix(-1,-1);

	int iSize = size();
	if((index > iSize) || (index<0))
		return pix;

	int iCount=0;
	pIter = begin();

	while((pIter !=end()) && (iCount!=index))
	{
		iCount++;
		++pIter;
	}
	
	pix = *pIter;
	return pix;
}

void CPixelList::Clear()
{
	CPixelList::iterator pIter;

	pIter = begin();
	while(pIter !=end())
	{
		this->remove (*pIter);
		pIter++;
	}
	
	return;	
}

void CPixelList::Serialize(CArchive &ar)
{
    if(ar.IsLoading())
    {
        clear();

        int nSize;
        ar >> nSize;
    
        CPixel pix;
        while((--nSize)>=0)
        {
            ar >> pix.x >> pix.y;
            push_back(pix);
        }
    }
    else
    {
        ar << size();
        
    	CPixelList::iterator pIter, pEnd;
        pIter = begin();
        pEnd = end();
        while(pIter!=pEnd)
        {
            ar << pIter->x << pIter->y;
            ++pIter;
        }
    }

    return;
}


CAdvImage * CPixelList::ContourToImage()
{
	int nLeft, nTop, nRight, nBottom;
	nLeft = nTop = INT_MAX;
	nRight = nBottom = 0;

	CPixelList::iterator Iter=begin();
    CPixelList::iterator IterEnd=end();
	for(; Iter!=IterEnd; ++Iter)
    {
        if(Iter->x > nRight)
			nRight = Iter->x;
		else if(Iter->x < nLeft)
			nLeft = Iter->x;

		if(Iter->y > nBottom)
			nBottom = Iter->y;
		else if(Iter->y < nTop)
			nTop = Iter->y;
	}

	int nWidth = nRight - nLeft + 1;
	int	nHeight = nBottom - nTop + 1;
	CAdvImage * pImg = new CAdvImage;
    pImg->CreateGrayScale(nWidth, nHeight);
    if(NULL == pImg->mImage)
    {
        delete pImg;
        return NULL;
    }
	
    unsigned char** destR = (unsigned char**)pImg->GetBand(0);
	for(Iter=begin(); Iter!=IterEnd; ++Iter)
	{
		Iter->y -= nTop;
		Iter->x -= nLeft;

		destR[Iter->y][Iter->x]=255;
	}

    for(int x=0; x<nWidth; x++)
    {
        int nyTop, nyBtm, y;
        nyTop = -1;
        for(y=0; y<nHeight; y++)
        {
            if(destR[y][x]==255)
            {
                nyTop = nyBtm = y;
                break;
            }
        }

        if(nyTop<0)
            continue;

        for(y=nHeight-1; y>nyTop; y--)
        {
            if(destR[y][x]==255)
            {
                nyBtm = y;
                break;
            }
        }                

        for(int tmp=nyTop; tmp<=nyBtm; tmp++)
        {
            destR[tmp][x] = 255;
        }
    }

	return pImg;
}