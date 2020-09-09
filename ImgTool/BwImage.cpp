#include "stdafx.h"
#include "../BwImage.h"

using namespace ipt;

CIptBwImage::CIptBwImage()
{
}


CIptBwImage& CIptBwImage::vector2img(const std::vector<IPTPOINT>& vec, 
                                     IPTPOINT ctr, int width, int height)
{
    if(0==width || 0==height)
    {
        IPTPOINT  tr = topright(vec);
        IPTPOINT  bl = bottomleft(vec);

        width  = tr.x - bl.x + 1;
        height = tr.y - bl.y + 1; 
    }

    if(createImg(width, height))
    {
        fill(0);

        std::vector<IPTPOINT>::const_iterator iter, iter_end;
        iter = vec.begin();
        iter_end = vec.end();

        IPTPIXEL pxl;
        while(iter != iter_end)
        {
            if(isValid(pxl.toPixel(*iter, ctr)))
                at(pxl) = 1;

            ++iter;
        }
    }

    return *this;
}


CIptBwImage& CIptBwImage::vector2img(const std::vector<IPTPIXEL>& vec, 
                                     IPTPIXEL ctr, int width, int height)
{
    if(0==width || 0==height)
    {
        IPTPIXEL  tl = topleft(vec);
        IPTPIXEL  br = bottomright(vec);

        width  = br.iCol - tl.iCol + 1;
        height = br.iRow - tl.iRow + 1; 
    }

    if(createImg(width, height))
    {
        fill(0);

        std::vector<IPTPIXEL>::const_iterator iter, iter_end;
        iter = vec.begin();
        iter_end = vec.end();

        IPTPIXEL pxl;
        while(iter != iter_end)
        {
            pxl = *iter + ctr;
            if(isValid(pxl))
                at(pxl) = 1;

            ++iter;
        }
    }

    return *this;
}


int CIptBwImage::countForegroundPixel() const
{
    const_iterator iter, iter_end;
    iter = begin();
    iter_end = end();

    int ret = 0;
    while(iter != iter_end)
    {
        if(*iter == 1)
            ret++;

        ++iter;
    }

    return ret;
}


CIptBwImage& CIptBwImage::convexFill()
{
    int nRows = rows();
    int nCols = columns();

    row_iterator  riter;

    for(int iRow=0; iRow<nRows; iRow++)
    {
        int iColL, iColR;

        riter = row_begin(iRow);

        iColL=0;
        while(iColL<nCols && riter[iColL]==0)
        {
            iColL++;
        }

        iColL++;
        while(iColL<nCols && riter[iColL]==1)
        {
            iColL++;
        }

        iColR = nCols-1;
        while(iColR>=0 && riter[iColR]==0)
        {
            iColR--;
        }

        iColR--;
        while(iColR>=0 && riter[iColR]==1)
        {
            iColR--;
        }

        while(iColL<=iColR)
        {
            riter[iColL] = 1;
            iColL++;
        }
    }

    return *this;
}


void CIptBwImage::contour()
{
    Const_Pixel_blk_iterator    itr = blk_begin();

    int imHeight = height();
    int imWidth  = width();

/*
	CPixel startPoint;
	//1. Find a point on the edge as the start point
	BOOL bFound = FALSE;
	for(int j=imHeight-1 ; j>=0; j--)
	{
		for(int i=imWidth-1; i>=0; i--)
		{
			if(mSliceImage->GetPixelAt(i,j,0)!=0)
			{
				startPoint.x = i;
				startPoint.y = j;
				bFound = TRUE;
				break;
			}
		}
		if(bFound)
			break;
	}

	//2. begin contour tracing
	int direction;
	direction = 1;
	
	CPoint firstPoint;
	firstPoint.x = startPoint.x;
	firstPoint.y = startPoint.y;
	while(1)
	{
		int	dir;

		if(isEVEN(direction))
			direction = MOD(direction+7,8);
		else
			direction = MOD(direction+6,8);

		for(int i=0;i<8;i++)
		{
			dir=MOD(i+direction,8);
			
			CPixel pixel = GetNeighbor(startPoint,dir);
			if( (pixel.x<0) || 
				(pixel.x>imWidth-1)||
				(pixel.y<0) ||
				(pixel.y>imHeight-1))
				continue;

			if((pixel.x == firstPoint.x)&&(pixel.y == firstPoint.y))
				return;

			
			else if(mSliceImage->GetPixelAt (pixel.x,pixel.y,0)>0)
			{
				if((pixel.x==startPoint.x)&&(pixel.y==startPoint.y))
					return;

				if(m_pSliceColorImage)
					m_DrawParameter.RotContour.push_back(startPoint);
				else
					m_DrawParameter.Contour.push_back(startPoint);

				startPoint=pixel;
				break;
			}
		}

		direction = dir;
	}
*/
	return;
}